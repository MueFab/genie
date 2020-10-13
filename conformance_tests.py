#!/usr/bin/env python3

import argparse
import logging as log
import os
import subprocess
import sys

avail_log_levels = {
    'critical': log.CRITICAL,
    'error': log.ERROR,
    'warning': log.WARNING,
    'info': log.INFO,
    'debug': log.DEBUG,
}


def get_git_root():
    return subprocess.Popen(['git', 'rev-parse', '--show-toplevel'], stdout=subprocess.PIPE).communicate()[0].rstrip().decode('utf-8')


def fastq_tests():
    git_root = get_git_root()

    fastq_folder = os.path.join(git_root, "data/fastq")
    fastq_files = [
        os.path.join(fastq_folder, "fourteen-records.fastq"),
        os.path.join(fastq_folder, "minimal.fastq"),
    ]

    for fastq_file in fastq_files:
        log.info('testing conformance with: {}'.format(fastq_file))


def main():
    # Basic log config
    format_string = '[%(asctime)s] [%(filename)25s:%(funcName)25s] [%(levelname)-8s] --- %(message)s'
    log.basicConfig(format=format_string, level=log.INFO)

    # Argument parser
    parser = argparse.ArgumentParser(description='conformance tests')
    parser.add_argument('-l', '--log_level', help='log level', choices=avail_log_levels.keys(), default='info')
    args = parser.parse_args()

    # Log level
    try:
        log_level = avail_log_levels[args.log_level]
    except KeyError:
        log.warning("invalid log level '%s' (using fall-back log level 'info')", args.log_level)
        log_level = log.INFO
    logger = log.getLogger()
    logger.setLevel(log_level)

    # Print banner
    log.info('********************************************************************************')
    log.info('    Genie Conformance Tests')
    log.info('********************************************************************************')

    # Log the command line
    log.debug('command line: %s', ' '.join(sys.argv))

    # Log the arguments
    log.debug('arguments: ')
    for arg in vars(args):
        log.debug('  %-16s: %s', arg, getattr(args, arg))


    fastq_tests()


if __name__ == "__main__":
    main()
