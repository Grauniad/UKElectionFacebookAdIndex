from dataclasses import dataclass

from mypy_boto3_dynamodb.service_resource import Table, DynamoDBServiceResource
from boto3.dynamodb.table import BatchWriter
from datetime import timezone, datetime, timedelta
from enum import Enum
from boto3.dynamodb.conditions import Key
from typing import Optional
import boto3
import uuid


def make_timestamp(time: datetime = None) -> str:
    if not time:
        time = datetime.now(timezone.utc)
    return time.strftime("%Y%m%d %H:%M:%S")


def get_db_resource() -> DynamoDBServiceResource:
    return boto3.resource('dynamodb', 'eu-west-2')


def get_table_resource() -> Table:
    return get_db_resource().Table("authentication")


def disable_user(user: str, writer: BatchWriter):
    writer.put_item({
        "PK": "User_" + user,
        "SK": "user",
        "max_logins": 0,
        "expiry_date": ""
    })


@dataclass
class Token:
    expiry_date: str
    token: str
    user: str


def _get_tokens_for_user(user_name: str) -> list[Token]:
    tokens: list[Token] = []
    response = \
        get_table_resource().query(
            Select="SPECIFIC_ATTRIBUTES",
            ConsistentRead=False,
            ProjectionExpression="SK, expiry_date, user_name",
            KeyConditionExpression=Key('PK').eq("Token_" + user_name))
    if "Items" in response:
        for row in response["Items"]:
            tokens.append(Token(expiry_date=row["expiry_date"],
                                token=row["SK"],
                                user=row["user_name"]))
    return tokens


def delete_token(token: Token, writer: BatchWriter):
    writer.delete_item({
        "PK": "Token_" + token.user,
        "SK": token.token
    })


def delete_all_tokens(user: str, writer: BatchWriter):
    tokens = _get_tokens_for_user(user)
    for token in tokens:
        delete_token(token, writer)


#
# Check the total number of tokens assigned to the user. Garbage collect
# any which are stale, and return the remaining valid tokens
#
def tidy_tokens(user_name: str, writer: BatchWriter) -> int:
    tokens = _get_tokens_for_user(user_name)
    count = 0
    now = make_timestamp()
    for token in tokens:
        if token.expiry_date < now:
            print(f'Expiring: {token}')
            delete_token(token, writer)
        else:
            print(f'Persisting: {token}')
            count += 1

    return count


def update_token(token: Token, writer: BatchWriter):
    item = {
        "PK": "Token_" + token.user,
        "SK": token.token,
        "user_name": token.user,
        "expiry_date": token.expiry_date
    }
    writer.put_item(item)


def lookup_token(token_id: str, user_name: str) -> Optional[Token]:
    # TODO: Requiring user is silly, and is due to a keying issue on
    #       our table we should clean this up when we've go the main
    #       data flow going
    key = {
        "PK": "Token_" + user_name,
        "SK": token_id
    }
    token = None
    response = get_table_resource().get_item(Key=key)
    if "Item" in response:
        item = response["Item"]
        token = Token(expiry_date=item["expiry_date"],
                      token=item["SK"],
                      user=item["user_name"])
    return token


def generate_token(user_name: str, expire_s: int, writer: BatchWriter) -> Token:
    now = datetime.now(timezone.utc)
    delta = timedelta(seconds=expire_s)
    token = Token(
        token=str(uuid.uuid4()),
        user=user_name,
        expiry_date=make_timestamp(now + delta))
    update_token(token, writer)
    return token


@dataclass
class AccessLevel(Enum):
    READ_ONLY = "READ_ONLY"
    NONE = "NONE"



def read_access_level(text: str) -> AccessLevel:
    if text == "READ_ONLY":
        return AccessLevel.READ_ONLY
    else:
        return AccessLevel.NONE


@dataclass
class InternalAuthContext:
    access_level: AccessLevel
    pass_thru: str


def extract_auth_context(event: dict) -> InternalAuthContext:
    auth_context = event.get("requestContext", {}).get("authorizer", {}).get("lambda", {})
    if auth_context:
        return InternalAuthContext(
            access_level=read_access_level(auth_context.get("ad_api_access_level", "")),
            pass_thru=auth_context.get("pass_thru", "")
        )
    else:
        return InternalAuthContext(
            access_level=AccessLevel.NONE
        )
