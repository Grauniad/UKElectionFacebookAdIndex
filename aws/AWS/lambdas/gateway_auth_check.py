from auth_common import AccessLevel, lookup_token, make_timestamp


def lambda_handler(event, context):
    token_id: str = event.get("headers", {}).get("ad_api_auth_token", "")
    user_name: str = event.get("headers", {}).get("ad_api_auth_user", "")
    now = make_timestamp()
    token = lookup_token(token_id, user_name)
    authorized = False
    level = AccessLevel.NONE
    if token and token.expiry_date > now:
        authorized = True
        level = AccessLevel.READ_ONLY

    return {
        "isAuthorized": authorized,
        "context": {
            "ad_api_access_level": level,
            "pass_thru": f"Your token was {token}"
        }
    }
