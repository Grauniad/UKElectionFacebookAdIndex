
from dataclasses import dataclass
from typing import Optional
from enum import Enum
from auth_common import get_table_resource, make_timestamp, tidy_tokens, generate_token, Token
import json


@dataclass
class ValidatedUser:
    name: str
    max_logins: int
    expiry: str


def validate_user(user_name: str, password: str) -> Optional[ValidatedUser]:
    validated_user = None
    response = get_table_resource().get_item(Key={
        'PK': "User_" + user_name,
        'SK': "user"
    })
    if "Item" in response:
        item = response["Item"]
        if "password" in item and item["password"]==password:
            validated_user = ValidatedUser(
                name=user_name, max_logins=item["max_logins"], expiry=item["expiry_date"]
            )
    return validated_user


class LoginStatus(Enum):
    AUTHENTICATED = "A"
    INVALID_LOGIN = "I"
    MAX_LOGINS_EXCEEDED = "M"
    USER_EXPIRED = "E"


@dataclass
class LoginResult:
    status: LoginStatus
    token: Optional[Token]


def login(user_name: str, password: str) -> LoginResult:
    with get_table_resource().batch_writer() as writer:
        login_result = LoginResult(status=LoginStatus.INVALID_LOGIN, token=None)
        validated_user = validate_user(user_name, password)
        num_tokens = tidy_tokens(user_name, writer)
        now = make_timestamp()

        if not validated_user:
            login_result.status = LoginStatus.INVALID_LOGIN
        elif validated_user.expiry < now:
            login_result.status = LoginStatus.USER_EXPIRED
        elif validated_user.max_logins <= num_tokens:
            login_result.status = LoginStatus.MAX_LOGINS_EXCEEDED
        else:
            login_result.token = generate_token(user_name, 3000, writer)
            login_result.status = LoginStatus.AUTHENTICATED

    return login_result


def lambda_handler(event, context):
    user_name: str = event.get("headers", {}).get("ad_api_auth_user", "")
    user_password: str = event.get("headers", {}).get("ad_api_auth_pass", "")
    result = login(user_name, user_password)
    if result.status == LoginStatus.AUTHENTICATED:
        return {
            'statusCode': 201,
            'headers': {
                'Content-Type': 'application/json',
                'Access-Control-Allow-Origin': '*',
                'ad_api_auth_token': result.token.token
            },
            "body": json.dumps({ 'ad_api_auth_token': result.token.token })
        }
    else:
        return {
            'statusCode': 403,
            'headers': {
                'Content-Type': 'application/json',
                'Access-Control-Allow-Origin': '*',
                'error_type': result.status.name
            }
        }
