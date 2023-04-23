#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json
import os
import re
import requests
import time
import getpass
import argparse;
import platform
import yaml
import fileinput

# import webbrowser
import subprocess
# import dateutil.parser
# import datetime
import termcolor
import sys

#
#
#
class CustomArgumentParser(argparse.ArgumentParser):
    def format_help(self):
        return argparse.ArgumentParser.format_help(self);#.replace('{%s}' % (','.join(Git.CommitTypes)), "TYPE");

#
#
#
class CommandLine:
    ProgramName = "fst";
    dir_path = os.path.dirname(os.path.realpath(__file__));
    project_directory = os.path.join(dir_path, "..");
    build_directory = os.path.join(project_directory, "build");

    @staticmethod
    def get_platform_build_directory(platform):
        if platform:
            return os.path.join(CommandLine.build_directory, platform);
        else:
            return CommandLine.build_directory

    def __init__(self):
        """Return the pathname of the KOS root directory."""
        self.__parser = CustomArgumentParser(prog = CommandLine.ProgramName,
        description = 'git commit helpers',
        epilog = 'Copyright (c) 2022 Audiokinetic Inc.');

        subparsers = self.__parser.add_subparsers(dest="command");

        buildDescription = '''\
Tools to create standardized git commits.

Example:
     input: {pname} -j WG-12345 -t fix -s engine -b -m "Change in engine"
    output: git commit -m "WG-12345 fix(engine)!: Change in engine"

'''.format(pname=CommandLine.ProgramName);

# gcc -msse3 -dM -E - < /dev/null | egrep "SSE|AVX" | sort

        setupParser = subparsers.add_parser('setup', description = buildDescription, help='build libraries and tests');
        cleanParser = subparsers.add_parser('clean', description = buildDescription, help='build libraries and tests');
        cleanParser.add_argument('-p', '--platform', dest='platform', help="platform");

        updateParser = subparsers.add_parser('push_update', description = buildDescription, help='build libraries and tests');

        cmakeParser = subparsers.add_parser('cmake', description = buildDescription, help='build libraries and tests');
        cmakeParser.add_argument('-g', '--gen', dest='generator', default="", help="show what would be done, without making any changes");
        cmakeParser.add_argument('-c', '--config', dest='config', default="Debug", help="show what would be done, without making any changes");
        cmakeParser.add_argument('-p', '--platform', dest='platform', help="platform");

        buildParser = subparsers.add_parser('build', description = buildDescription, help='build libraries and tests');
        buildParser.add_argument('-c', '--config', dest='config', default="Debug", help="show what would be done, without making any changes");
        buildParser.add_argument('-p', '--platform', dest='platform', help="platform");

        testParser = subparsers.add_parser('test', description = buildDescription, help='build libraries and tests');
        testParser.add_argument('-c', '--config', dest='config', default="Debug", help="show what would be done, without making any changes");
        testParser.add_argument('-p', '--platform', dest='platform', help="platform");

    """ Copy tools/hooks/pre-commit hook to .git/hooks folder """
    @staticmethod
    def command_setup(args):
        file_to_copy = os.path.join(CommandLine.project_directory, "tools", "hooks", "pre-commit");
        dst = os.path.join(CommandLine.project_directory, ".git", "hooks");
        os.system('cp "{0}" "{1}"'.format(file_to_copy, dst));

    """ delete build directory """
    @staticmethod
    def command_clean(args):
        os.system('rm -rf "{0}"'.format(CommandLine.get_platform_build_directory(args.platform)));

    """ delete build directory """
    @staticmethod
    def command_push_update(args):
        cmd1 = 'git -C "{0}" add --all'.format(CommandLine.project_directory);
        cmd2 = 'git -C "{0}" commit -m "update"'.format(CommandLine.project_directory);
        cmd3 = 'git -C "{0}" push'.format(CommandLine.project_directory);

        os.system(cmd1);
        os.system(cmd2);
        os.system(cmd3);

    """ generate project """
    @staticmethod
    def command_cmake(args):
        options = ""
        # 
        if args.generator == "clang":
            options = '-DCMAKE_BUILD_TYPE={0} -G "Visual Studio 17 2022" -T ClangCL -A x64'.format(args.config);

        elif args.generator == "mingw":
            options = '-DCMAKE_BUILD_TYPE={0} -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_MAKE_PROGRAM=make -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -G "MinGW Makefiles"'.format(args.config);

        elif args.generator == "makefile":
            options = '-DCMAKE_BUILD_TYPE={0} -G "Unix Makefiles"'.format(args.config);

        elif platform.system() == "Windows":
            options = ""

        # os.mkdir(CommandLine.build_directory);
        os.makedirs(CommandLine.get_platform_build_directory(args.platform), exist_ok = True)
        cmd = 'cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=on -B{0} -S{1} {2}'.format(CommandLine.get_platform_build_directory(args.platform), CommandLine.project_directory, options);
        os.system(cmd);

    """ build project """
    @staticmethod
    def command_build(args):
        cmd = "cmake --build {0} --config {1}".format(CommandLine.get_platform_build_directory(args.platform), args.config);
        os.system(cmd);

    @staticmethod
    def command_test(args):
        # cmd = 'ctest --verbose --output-on-failure --test-dir "{0}" -C {1}'.format(CommandLine.get_platform_build_directory(args.platform), args.config);
        cmd = 'ctest --output-on-failure --test-dir "{0}" -C {1}'.format(CommandLine.get_platform_build_directory(args.platform), args.config);
        os.system(cmd);
        
    def PrintHelp(self):
        self.__parser.print_help();

    def Run(self):
        args = self.__parser.parse_args();

        if args.command:
            commandMethod = getattr(CommandLine, "command_" + args.command);
            commandMethod(args);
        else:
            self.PrintHelp();

def is_header_missing(f):
    with open(f) as reader:
        lines = reader.read().lstrip().splitlines()
        if len(lines) > 0:
            return not lines[1].startswith("/// BSD 3-Clause License")
        return True

def add_headers(files, header):
    for line in fileinput.input(files, inplace=True):
        if fileinput.isfirstline():
            [ print(h) for h in header.splitlines() ]
        print(line, end="")
        
def add_file_header(filepath):
    print(filepath);
    print(is_header_missing(filepath))
    if is_header_missing(filepath):
        print("KK")
        with open(filepath, 'r') as original: data = original.read()
        with open(filepath, 'w') as modified: modified.write("///\n" + data)
        

def parse_config():
    config_path = os.path.join(CommandLine.project_directory, ".config");
    fconfig = open(config_path, "r+");
    fdata = yaml.safe_load(fconfig);
    print(fdata);

    for key in fdata:
        # print(type(fdata[key]))

        if type(fdata[key]) is bool:
            print("#define " + key + " " + str(int(fdata[key])))
        else:
            print("#define " + key + " " + str(fdata[key]))
# 
#
#
#
if __name__ == "__main__":
    print('platform {0}'.format(platform.system()))
    
    # include_dir = os.path.join(CommandLine.project_directory, "include", "fst");
    # # print(include_dir)
    # # paths = [os.path.join(include_dir, fn) for fn in next(os.walk(include_dir))[2]]
    # # paths = os.listdir(include_dir);
    # # print(paths)
    # res = [];
    # for path in os.listdir(include_dir):
    #     # check if current path is a file
    #     if os.path.isfile(os.path.join(include_dir, path)):
    #         res.append(path)
    #         add_file_header(os.path.join(include_dir, path));
    # print(res)
        
    # parse_config();
    # with open(CommandLine.project_directory + "/.config", 'r') as stream:
    #     data_loaded = yaml.safe_load(stream)
    #     print(data_loaded)
    #     print(data_loaded["FST_USE_STL"])
        
    #     for key in data_loaded:
    #         print("#define " + key + " " + str(data_loaded[key]))
        
    #     config_file_path = os.path.join(CommandLine.project_directory, "tools", "config.h.in")
    #     cfile = open(config_file_path, "r+");
    #     cfile_data = cfile.read();
    #     print(cfile_data)
    #     # with open("demofile2.txt", "w+") as f:
    #     #     data = f.read()
    #     #     print(data)
    #     #     f.seek(0)
    #     #     f.write("demofile2.txt")
    #     #     f.truncate()
            
    #     f = open("demofile2.txt", "w+")
    #     # print(f.read())

    #     # f.write("Now the file has more gg!")
    #     f.close()

    # print(data == data_loaded)
    # print(dir_path)
    # print(Git.IsGitDirectory())
    # print(Git.IsGitDirectory("C:\\Code\\Wwise"))
    # os.system();
    # ddd = subprocess.check_output('cygpath --absolute --long-name --windows "/c/Code/"', shell=False).decode("utf-8");
    # print(re.sub('\n', '', ddd), "a")
    cmd = CommandLine();
    cmd.Run();
