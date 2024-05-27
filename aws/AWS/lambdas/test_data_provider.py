import json
from auth_common import extract_auth_context


def lambda_handler(event, context):
    auth_context = extract_auth_context(event)
    message = {
        'message': 'We automated this deployment!',
        'pass-thru': auth_context.pass_thru,
        'context': auth_context.access_level.name
    }
    return {
        'statusCode': 200,
        'headers': {
            'Content-Type': 'application/json',
            'Access-Control-Allow-Origin': '*',

        },
        'body': json.dumps(message)
    }

