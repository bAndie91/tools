
from __future__ import print_function

import json
import urllib
import boto3
import traceback

print('Loading function')

s3 = boto3.client('s3')
iam = boto3.client('iam')


def lambda_handler(event, context):
	#print("Received event: " + json.dumps(event, indent=2))
	
	Bucket = event['Records'][0]['s3']['bucket']['name']
	Key = urllib.unquote_plus(event['Records'][0]['s3']['object']['key'].encode('utf8')).strip('/')
	UserId = None
	Identity = event['Records'][0]['userIdentity']['principalId']
	if Identity.startswith('AWS:'):
		if Identity[4:].find(':')==-1:
			UserId = Identity[4:]
	else:
		UserId = Identity
	Tagname = "Creator"
	Tagvalue = None
	
	try:
		if UserId is None:
			raise Exception("PrincipalId '%s' is not a UserId" % (Identity))
		else:
			userlist = iam.list_users()
			for user in userlist['Users']:
				if user['UserId'] == UserId:
					Tagvalue = user['UserName']
					break
			if Tagvalue is None:
				raise Exception("No UserName found for UserId '%s' %s" % (Identity, repr(userlist)))
	except:
		traceback.print_exc()
		Tagname = "CreatorIdentity"
		Tagvalue = Identity
	
	Tagging = {
		'TagSet': [
			{'Key': Tagname, 'Value': Tagvalue},
		]
	}
	
	s3.put_object_tagging(Bucket=Bucket, Key=Key, Tagging=Tagging)
