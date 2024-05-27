import sys
import json
import zlib
from dataclasses import dataclass
from boto3.dynamodb.table import BatchWriter
from mypy_boto3_dynamodb.client import DynamoDBClient
from mypy_boto3_dynamodb.service_resource import Table, DynamoDBServiceResource
from boto3.dynamodb.conditions import Key
from typing import Optional
import boto3


def get_db_connection() -> DynamoDBClient:
    return boto3.client('dynamodb', 'eu-west-2')


def get_db_resource() -> DynamoDBServiceResource:
    return boto3.resource('dynamodb', 'eu-west-2')


def get_table_resource() -> Table:
    return get_db_resource().Table("ads_db")


@dataclass
class StoredAd:
    id: str
    captured_date: str
    data: str


def store_ad(ad: StoredAd, writer: BatchWriter):
    item = {
        "PK": "Stored_Ad_" + ad.id,
        "id:": ad.id,
        "SK": ad.captured_date,
        "ad_data": zlib.compress(ad.data.encode())
    }
    writer.put_item(item)


def retrieve_ad(ad_id: str) -> Optional[StoredAd]:
    stored_ad = None
    response = \
        get_table_resource().query(
            Select="SPECIFIC_ATTRIBUTES",
            Limit=1,
            ScanIndexForward=False,
            ConsistentRead=False,
            ProjectionExpression="id, SK, ad_data",
            KeyConditionExpression=Key('PK').eq("Stored_Ad_" + ad_id))
    if "Items" in response:
        item = response["Items"][0]
        data = zlib.decompress(item["ad_data"].value).decode()
        stored_ad = StoredAd(
            id=ad_id,
            captured_date=item["SK"],
            data=data
        )
    return stored_ad
