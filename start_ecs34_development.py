#!/usr/bin/env python3
from __future__ import annotations 
from typing import List

import os
import sys
import subprocess
import argparse

class DockerManager:

    IMAGE_NAME      = 'ecs34_img'
    CONTAINER_NAME  = 'ecs34_dev'

    def __init__(self):
        pass

    @staticmethod
    def script_directory() -> str:
        return str(os.path.dirname(os.path.realpath(__file__)))

    @staticmethod
    def split_columns(line : str) -> List[str]:
        new_line = line.strip().replace('  ','\t').replace('\t\t','\t')
        while line != new_line:
            line = new_line
            new_line = new_line.replace('\t ','\t').replace('\t\t','\t')
        return line.split('\t')

    @staticmethod
    def running_in_docker_container() -> bool:
        return os.path.exists('/.dockerenv')
    
    @staticmethod
    def image_exists() -> bool:
        with subprocess.Popen(['docker','images','-q',DockerManager.IMAGE_NAME], stdout=subprocess.PIPE) as docker_images:
            return bool(len(docker_images.stdout.readlines()))

    @staticmethod
    def container_running() -> bool:
        with subprocess.Popen(['docker','ps','-q','-f',f'name={DockerManager.CONTAINER_NAME}'], stdout=subprocess.PIPE) as docker_ps:
            return bool(len(docker_ps.stdout.readlines()))

    @staticmethod
    def container_exited() -> bool:
        with subprocess.Popen(['docker','ps','-aq','-f','status=exited','-f',f'name={DockerManager.CONTAINER_NAME}'], stdout=subprocess.PIPE) as docker_ps:
            return bool(len(docker_ps.stdout.readlines()))

    @staticmethod
    def build_image() -> bool:
        print(f'{DockerManager.IMAGE_NAME}, does not exist, building:')
        dockerfile_path = os.path.join(DockerManager.script_directory(),'Dockerfile')
        if not os.path.exists(dockerfile_path):
            print(f'Dockerfile not found in {DockerManager.script_directory()}. Dockerfile must be in same directory as launch script!')
            return False
        command = ['docker','build','--load','-t',DockerManager.IMAGE_NAME,'-f',dockerfile_path,DockerManager.script_directory()]
        print(' ',' '.join(command))
        with subprocess.Popen(command, stdout=subprocess.PIPE) as docker_build:
            for line_index,line in enumerate(docker_build.stdout.readlines()):
                print(line_index,line.decode('utf-8'))
            return not docker_build.returncode

    @staticmethod
    def create_container() -> bool:
        print(f'{DockerManager.CONTAINER_NAME}, does not exist, running:')
        command = ['docker','run','-it','-v', f'{DockerManager.script_directory()}:/workspace','--name',DockerManager.CONTAINER_NAME,DockerManager.IMAGE_NAME]
        print(' ',' '.join(command))
        with subprocess.Popen(command) as docker_run:
            return True

    @staticmethod
    def restart_container() -> bool:
        print(f'{DockerManager.CONTAINER_NAME}, not running, restarting:')
        command = ['docker','restart',DockerManager.CONTAINER_NAME]
        print(' ',' '.join(command))
        with subprocess.Popen(command) as docker_run:
            return True
    
    @staticmethod
    def mounted_directory() -> str:
        command = ['docker','inspect',"--format={{(index .Mounts 0).Source}}",DockerManager.CONTAINER_NAME]
        with subprocess.Popen(command, stdout=subprocess.PIPE,text=True) as docker_inspect:
            return docker_inspect.communicate()[0].strip()
        
    @staticmethod
    def running_from_mounted() -> bool:
        mounted_dir = DockerManager.mounted_directory() 
        script_dir =DockerManager.script_directory()
        return mounted_dir == script_dir

    @staticmethod
    def exec_bash() -> bool:
        print(f'Entering {DockerManager.CONTAINER_NAME} bash:')
        command = ['docker','exec','-it',DockerManager.CONTAINER_NAME,'bash']
        print(' ',' '.join(command))
        with subprocess.Popen(command) as docker_run:
            return True

def main(*argv):
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument('-v','--verbose',action='store_true')
    arguments = arg_parser.parse_args(list(argv))

    if DockerManager.running_in_docker_container():
        print('Already in docker container!')
        return -1
    
    if not DockerManager.image_exists():
        if not DockerManager.build_image():
            print('Failed to build image.')
            return -1
    
    if not DockerManager.container_running():
        print('Container not running')

        if not DockerManager.container_exited():
            DockerManager.create_container()
            return 0
        DockerManager.restart_container()

    print(f'Your current shell script is running in {DockerManager.script_directory()}')
    if not DockerManager.running_from_mounted():
        print(f'''ERROR!: The file you are running is NOT in your MOUNTED Volume!
ERROR!: The file you are running is NOT in your MOUNTED Volume!
ERROR!: The file you are running is NOT in your MOUNTED Volume!
ERROR!: The file you are running is NOT in your MOUNTED Volume!
    
************************************************************************************************
Options to correct the error:
(1) Move all your work files from the location of this script to your mounted path: {DockerManager.mounted_directory()}
---- OR ----
(2) Stop the container in your Docker application, remove the container, re-run this script.
************************************************************************************************''')
        return -1
    DockerManager.exec_bash()
            

    return 0


if __name__ == "__main__":
    sys.exit(main(*sys.argv[1:]))