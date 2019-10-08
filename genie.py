#!/usr/bin/env python

from __future__ import print_function
import os
import sys
try:
    from shlex import quote as cmd_quote
except ImportError:
    from pipes import quote as cmd_quote


valid_tools = ['encapsulator',
               'gabac-app',
               'global-assembly-encoder',
               'local-assembly-encoder',
               'reference-based encoder',
               'ureads-encoder']


def print_info(*args, **kwargs):
    print("[genie]", *args, file=sys.stdout, **kwargs)


def print_error(*args, **kwargs):
    print("[genie] error:", *args, file=sys.stderr, **kwargs)


def print_help():
    print_info('usage: genie TOOL ARGS')
    print_info()
    print_info('options:')
    print_info('  TOOL    tool to execute, available tools are:')
    for tool in valid_tools:
        print_info('            {}'.format(tool))
    print_info('  ARGS    arguments to the selected TOOL')


def main():
    # get the command line
    cmdline = " ".join(map(cmd_quote, sys.argv[1:]))

    # check whether we need to print the help
    tool = cmdline.partition(' ')[0]
    if tool == "" or tool == '-h' or tool == '--help' or tool == 'help':
        print_help()
        sys.exit(0)

    # check whether the selected tool is valid
    if tool not in valid_tools:
        print_error("invalid tool: {} (try 'genie help')".format(tool))
        sys.exit(1)

    # execute
    script_abspath = os.path.abspath(os.path.dirname(sys.argv[0]))
    bin_dir = script_abspath + '/cmake-build-release/bin/'
    if not os.path.exists(bin_dir):
        print_error('binary directory does not exist: {} (did you build Genie?)'.format(bin_dir))
        sys.exit(1)
    print_info('executing: {}'.format(cmdline))
    os.system(bin_dir + cmdline)


if __name__ == "__main__":
    main()
