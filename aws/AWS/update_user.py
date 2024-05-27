import sys

from dataclasses import dataclass

from mypy_boto3_dynamodb.service_resource import Table, DynamoDBServiceResource
import boto3


def get_db_resource() -> DynamoDBServiceResource:
    return boto3.resource('dynamodb', 'eu-west-2')


def get_table_resource() -> Table:
    return get_db_resource().Table("authentication")


def usage():
    print("update_user <user_name> <new password> <login expiry (UTC):YYYYMMDD HH:MM:SS>")

@dataclass
class Args:
    user_name: str
    password: str
    expiry_date: str


def parse_args() -> Args:
    if len(sys.argv) != 4:
        print(usage())
        exit(1)
    return Args(user_name=sys.argv[1],
                password=sys.argv[2],
                expiry_date=sys.argv[3])


def main():
    args = parse_args()
    item = {
        "PK": "User_" + args.user_name,
        "SK": "user",
        "user_name": args.user_name,
        "password": args.password,
        "max_logins": 5,
        "expiry_date": args.expiry_date
    }
    with get_table_resource().batch_writer() as writer:
        writer.put_item(item)


if __name__ == "__main__":
    main()