#!/bin/bash


# Set your preferred profile name.
#AWS_DEFAULT_PROFILE=default
# Set S3 bucket names in which quota should be enabled.
# Leaving empty will enable quota on all buckets.
#Buckets="your_bucket_name1 your_bucket_name2 ..."
Buckets=
Rolename=lambda-s3quota
Funcname=quota-s3
lastlog=


logstart()
{
	echo "[33m★ $*[m" >&2
	lastlog="$*"
}
logend()
{
	local c=2 m=✔
	if [ "$1" -gt 0 ] 2>/dev/null || [ -z "$1" ]
	then
		c=1
		m=✘
	fi
	[ "$1" -ge 0 ] 2>/dev/null && shift
	echo "[3$c;1m$m $lastlog[m [3${c}m$*[m" >&2
}


## Create IAM Role

logstart Create Role $Rolename
aws iam create-role --role-name "$Rolename" --assume-role-policy-document file://assumerole-lambda.json
rolearn=`aws iam get-role --role-name "$Rolename" --query Role.Arn --output text`
logend "$rolearn"

## Create IAM Policies

logstart Create Policy IAMListUsers
aws iam create-policy --policy-name IAMListUsers --policy-document file://policy-listusers.json
logstart Create Policy S3PutObjectTagging
aws iam create-policy --policy-name S3PutObjectTagging --policy-document file://policy-s3puttag.json


## Attach Policies to Role

get_policy_arn()
{
	local policy=$1
	aws iam list-policies --query "Policies[?PolicyName==\`$policy\`].Arn" --output text
}

for policy in IAMListUsers S3PutObjectTagging AWSLambdaBasicExecutionRole
do
	logstart Attach Policy $policy to Role $Rolename
	arn=`get_policy_arn "$policy"`
	aws iam attach-role-policy --role-name "$Rolename" --policy-arn "$arn"
	logend "$arn"
done

aws iam list-attached-role-policies --role-name "$Rolename" --output text


## Create Cloudwatch Log Group
logstart Create Cloudwatch Log Group
aws logs create-log-group --log-group-name /aws/lambda/"$Funcname"
aws logs put-retention-policy --log-group-name /aws/lambda/"$Funcname" --retention-in-days 7


## Create Lambda function

logstart Create Lambda $Funcname
tmpfile=`mktemp --suffix=.zip -u`
trap 'rm "$tmpfile"' EXIT INT
zip "$tmpfile" lambda_function.py

aws lambda create-function --function-name "$Funcname" \
	--description "Listens to S3 write events and put 'Creator' tag on objects." \
	--role "$rolearn" \
	--runtime python2.7 \
	--handler lambda_function.lambda_handler \
	--zip-file "fileb://$tmpfile"

lambdaarn=`aws lambda get-function --function-name "$Funcname" --query Configuration.FunctionArn --output text`
logend "$lambdaarn"


## Grant S3 to invoke Lambda

logstart Permit S3 service to Lambda
aws lambda add-permission --function-name "$Funcname" --statement-id quota-s3 --action lambda:InvokeFunction --principal s3.amazonaws.com


## Create Lambda Trigger within S3

merge_notification_config()
{
	local bucket=$1
	export lambdaarn=$2
	export eventId="create-all-lambda"
	aws s3api get-bucket-notification-configuration --bucket "$bucket" |\
	python -c '
from __future__ import print_function
import json
import sys
import os
eventId = os.environ["eventId"]
cnfstr = "".join(sys.stdin.readlines())
sys.stderr.write("Current Notification Config: %s\n" % (cnfstr))
if cnfstr:
	config = json.loads(cnfstr)
else:
	config = {"LambdaFunctionConfigurations":[]}
Lambda = dict(map(lambda x: (x["Id"], x), config["LambdaFunctionConfigurations"]))
newLambda = {eventId: {
 "Id": eventId,
 "LambdaFunctionArn": os.environ["lambdaarn"],
 "Events": ["s3:ObjectCreated:*"]
}}
Lambda.update(newLambda)
config.update({"LambdaFunctionConfigurations": Lambda.values()})
print(json.dumps(config))
'
}

for bucket in `[ -n "$Bucket" ] && echo "$Buckets" || aws s3api list-buckets --query 'Buckets[*].Name' --output text`
do
	logstart Set Notification Config on bucket $bucket
	aws s3api put-bucket-notification-configuration --bucket "$bucket" --notification-configuration "$(merge_notification_config "$bucket" "$lambdaarn")"
	logend $?
done

