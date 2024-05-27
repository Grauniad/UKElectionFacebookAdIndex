import sys

from dataclasses import dataclass

from auth_common import get_table_resource, disable_user, delete_all_tokens


def usage():
    print("force_logout <user_name>")


@dataclass
class Args:
    user_name: str


def parse_args() -> Args:
    if len(sys.argv) != 2:
        print(usage())
        exit(1)
    return Args(user_name=sys.argv[1])


def main():
    args = parse_args()
    with get_table_resource().batch_writer() as writer:
        disable_user(args.user_name, writer)
        delete_all_tokens(args.user_name, writer)


if __name__ == "__main__":
    main()
