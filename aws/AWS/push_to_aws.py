import sys
import json
from dataclasses import dataclass
from mypy_boto3_dynamodb.client import DynamoDBClient
from mypy_boto3_dynamodb.service_resource import Table, DynamoDBServiceResource
from mypy_boto3_dynamodb.service_resource import T
import boto3


def get_db_connection() -> DynamoDBClient:
    return boto3.client('dynamodb', 'eu-west-2')


def get_db_resource() -> DynamoDBServiceResource:
    return boto3.resource('dynamodb', 'eu-west-2')


def get_table_resource() -> Table:
    return get_db_resource().Table("ads_db")



def get_PK(json_ad) -> str:
    return "Funder_" + json_ad["funder"]


def get_SK(json_ad) -> str:
    return json_ad["start_date"] + "-" + str(json_ad["id"])


def expand_to_aws_item_format(json: dict) -> dict:
    # We only have strings and numbers, so this is a lot simpler than the general
    # case. Don't bother validating - let AWS do that.
    aws = {}
    for name, value in json.items():
        if type(value) == type("this is a string"):
            aws[name] = {
                "S": value
            }
        else:
            aws[name] = {
                "N": str(value)
            }
    return aws


@dataclass
class Args:
    input_file: str


def usage() -> str:
    return "push_to_aws <export file>"


def parse_args() -> Args:
    if len(sys.argv) != 2:
        print(usage)
        exit(1)
    return Args(input_file=sys.argv[1])


def main():
    args = parse_args()
    with open(args.input_file) as input_file:
        data = json.load(input_file)['ads']
        with get_table_resource().batch_writer() as writer:
            for ad in data:
                ad['PK'] = get_PK(ad)
                ad['SK'] = get_SK(ad)
                writer.put_item(Item=ad)


if __name__ == "__main__":
    main()