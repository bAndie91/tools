
import botocore
import boto3
#import json
#import re
#import uuid
import time
from datetime import datetime
#import random


#boto3.setup_default_session(profile_name='sajat')

print('Loading function at ' + datetime.now().time().isoformat())
route53 = boto3.client('route53')
compute = boto3.client('ec2')
global_default_ttl = 3600
(UPDATE, DELETE) = range(2)
R53Cache = []


def lambda_handler(event, context):
  # print(repr(event))
  
  if event['detail'].has_key('eventName'):
    region = event['detail']['awsRegion']
    instance_id = event['detail']['requestParameters']['resourcesSet']['items'][0]['resourceId']  #FIXME iterate through on all instances
    if event['detail']['eventName'] == 'CreateTags':
      action = UPDATE
    elif event['detail']['eventName'] == 'DeleteTags':
      action = DELETE
  else:
    region = event['region']
    instance_id = event['detail']['instance-id']
    state = event['detail']['state']
    if state == 'running':
      action = UPDATE
    else:
      action = DELETE
  
  # DELETE old record if exists
  ZoneId = None
  try:
    (Name, ZoneId, Zone) = get_instance_name_by_id(instance_id)
  except:
    print("Old Name not found in DNS for %s" % instance_id)
    if action == DELETE:
      return False
  
  if ZoneId is not None:
    public_ip = ''
    for record in iter_records():
      if record['zone'] == Zone and record['type'] == 'A' and record['name'] == Name:
        public_ip = record['content']
        break
    
    dereg = deregister_name(ZoneId, Zone, Name, instance_id, public_ip)
    if action == DELETE:
      return dereg
  
  # Add new record
  if action == UPDATE:
    while True:
      try:
        instance = compute.describe_instances(InstanceIds=[instance_id])
        break
      except botocore.exceptions.ClientError:
        pass
    
    try:
      public_ip = instance['Reservations'][0]['Instances'][0]['PublicIpAddress']
    except BaseException as e:
      print("Instance %s has no public IP" % instance_id)
      return None
    
    tags = dict(map(lambda t: (t['Key'], t['Value']), instance['Reservations'][0]['Instances'][0]['Tags']))
    try:
      DnsName = tags['dns-name']
    except KeyError:
      try:
        Name = tags['Name']
        Zone = tags['dns-zone']
        if not Zone.endswith('.'):
          Zone += '.'
        try:
          ZoneId = get_zone_id_by_name(Zone)
        except ValueError:
          print("No such Zone with name '%s'" % Zone)
          return False
      except KeyError:
        print("Name and dns-zone tags or dns-name tag not found on instance %s" % instance_id)
        return None
    
    try:
      if not DnsName.endswith('.'):
        DnsName += '.'
      for record in sorted(iter_records(), cmp = lambda a, b: cmp(len(b['zone']), len(a['zone']))):
        suffix = '.' + record['zone']
        if DnsName.endswith(suffix):
          Name = DnsName[0:-len(suffix)]
          Zone = record['zone']
          ZoneId = record['zone_id']
          break
      try:
        Zone
      except NameError:
        print("Zone not found for FQDN '%s'" % DnsName)
        return False
    except NameError:
      pass
    
    return register_name(ZoneId, Zone, Name, instance_id, public_ip)


def iter_records():
  if len(R53Cache) > 0:
    for record_dict in R53Cache:
      yield record_dict
  else:
    zones = route53.list_hosted_zones()['HostedZones']
    for zone in zones:
      records = route53.list_resource_record_sets(HostedZoneId=zone['Id'])['ResourceRecordSets']
      for record in records:
        for rr in record['ResourceRecords']:
          zone_name = zone['Name']
          name = record['Name'][0:-len(zone_name)-1]
          type = record['Type']
          content = rr['Value']
          if type == 'TXT':
            content = content.strip('"')
          record_dict = {'zone': zone_name, 'zone_id': zone['Id'], 'type': type, 'name': name, 'ttl': record['TTL'], 'content': content}
          #R53Cache.append(record_dict)
          yield record_dict

def get_instance_name_by_id(instance_id):
  for record in iter_records():
    if record['type'] == 'TXT' and record['name'] == instance_id + '.instance-id':
      return (record['content'], record['zone_id'], record['zone'])
  raise ValueError("No such TXT record with name '%s'" % instance_id)

def get_zone_id_by_name(zone_name):
  for record in iter_records():
    if record['zone'] == zone_name:
      return record['zone_id']
  raise ValueError("No such zone with name '%s'" % zone_name)

def register_name(zone_id, zone, name, instance_id, content):
  fqdn = name + '.' + zone
  print("Registering: %s IN A %s, Id=%s" % (fqdn, content, instance_id))
  
  route53.change_resource_record_sets(
    HostedZoneId = zone_id,
    ChangeBatch = {
      "Comment": "Updated by Lambda DDNS",
      "Changes": [
        {
          "Action": "UPSERT",
          "ResourceRecordSet": {
            "Name": fqdn,
            "Type": 'A',
            "TTL": global_default_ttl,
            "ResourceRecords": [{"Value": content}]
          }
        },
        {
          "Action": "UPSERT",
          "ResourceRecordSet": {
            "Name": instance_id + '.instance-id.' + zone,
            "Type": 'TXT',
            "TTL": global_default_ttl,
            "ResourceRecords": [{"Value": '"' + name + '"'}]
          }
        }
      ]
    }
  )
  return True

def deregister_name(zone_id, zone, name, instance_id, content):
  fqdn = name + '.' + zone
  print("Deregistering: %s IN A %s, Id=%s" % (fqdn, content, instance_id))
  
  route53.change_resource_record_sets(
    HostedZoneId = zone_id,
    ChangeBatch = {
      "Comment": "Deleted by Lambda DDNS",
      "Changes": [
        {
          "Action": "DELETE",
          "ResourceRecordSet": {
            "Name": fqdn,
            "Type": 'A',
            "TTL": global_default_ttl,
            "ResourceRecords": [{"Value": content}]
          }
        },
        {
          "Action": "DELETE",
          "ResourceRecordSet": {
            "Name": instance_id + '.instance-id.' + zone,
            "Type": 'TXT',
            "TTL": global_default_ttl,
            "ResourceRecords": [{"Value": '"' + name + '"'}]
          }
        }
      ]
    }
  )
  return True

