import os
from dataclasses import dataclass
import shutil
import sys
import json
from mypy_boto3_lambda.client import LambdaClient
from typing import Optional
import boto3


def usage():
    print("make_build.py <build_cfg>")
    exit(1)


@dataclass
class UploadConfig:
    region: str
    lambda_name: str


@dataclass
class Args:
    builds_dir: str
    build_cfg: str


def parse_args() -> Args:
    if len(sys.argv) != 2:
        usage()
        exit(1)
    return Args(build_cfg=sys.argv[1],
                builds_dir=os.path.abspath(os.path.dirname(sys.argv[0])))


@dataclass
class ImportFolder:
    src_path: str
    name: str


@dataclass
class SrcFile:
    src_path: str
    name: str


@dataclass
class Config:
    folders: list[ImportFolder]
    src_files: list[SrcFile]
    upload: Optional[UploadConfig]


def parse_config(args: Args) -> Config:
    config = Config(folders=[], src_files=[], upload=None)
    with open(args.build_cfg) as config_file:
        config_json = json.loads(config_file.read())
        print(config_json)
        for folder_map in config_json.get("import_folders", []):
            dest_name = folder_map.get("name", "")
            config.folders.append(ImportFolder(
                src_path=folder_map["src"],
                name=dest_name
            ))
        for file_map in config_json.get("src_files", []):
            dest_name = file_map.get("name", "")
            config.src_files.append(SrcFile(
                src_path=file_map["src"],
                name=dest_name
            ))
        if "upload" in config_json:
            upload_json = config_json["upload"]
            config.upload = UploadConfig(region=upload_json["region"],
                                         lambda_name=upload_json["name"])

    print(config)
    return config


def get_working_dir(args: Args) -> str:
    return os.path.join(args.builds_dir, "working")


def get_target_path(args: Args, object_to_move: SrcFile | ImportFolder) -> str:
    name = object_to_move.name
    if name == "":
        name = os.path.basename(object_to_move.src_path)
    return os.path.join(get_working_dir(args), name)


def prepare_workspace(args: Args):
    working_dir = get_working_dir(args)
    if os.path.exists(working_dir):
        shutil.rmtree(working_dir)
    os.mkdir(working_dir)


def copy_in_contents(args: Args, config: Config):
    for dir in config.folders:
        shutil.copytree(dir.src_path, get_target_path(args, dir))

    for src in config.src_files:
        shutil.copy(src.src_path, get_target_path(args, src))


def make_zip(args: Args):
    shutil.make_archive("build_output", "zip", root_dir=get_working_dir(args))


def upload_to_aws(config: UploadConfig):
    client: LambdaClient = boto3.client('lambda', config.region)
    target_file_name = "build_output" + ".zip"
    client.update_function_code(FunctionName=config.lambda_name, ZipFile=open(target_file_name, 'rb').read())


def main():
    args = parse_args()
    config = parse_config(args)
    prepare_workspace(args)
    copy_in_contents(args, config)
    make_zip(args)
    if config.upload:
        upload_to_aws(config.upload)


if __name__ == "__main__":
    main()

