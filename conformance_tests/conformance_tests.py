#!/usr/bin/env python3

import argparse
import logging as log
import multiprocessing
import os
import shutil
import subprocess
import sys
import tempfile

_avail_log_levels = {
    'critical': log.CRITICAL,
    'error': log.ERROR,
    'warning': log.WARNING,
    'info': log.INFO,
    'debug': log.DEBUG,
}

_build_types = [
    # 'Debug',
    'Release',
]

_build_dir_prefix = 'cmake-build-conformance-tests-'


class InternalError(Exception):
    pass


def _get_git_root():
    git_root = subprocess.Popen(['git', 'rev-parse', '--show-toplevel'], stdout=subprocess.PIPE).communicate()[0].rstrip().decode('utf-8')

    if not git_root.endswith('genie'):
        raise InternalError('this script must be started from the Genie repository')

    return git_root


def _build(num_threads: int):
    git_root = _get_git_root()
    executables = {}

    for build_type in _build_types:
        log.info("building build type '" + build_type + "'")
        curr_build_dir = os.path.join(git_root, _build_dir_prefix + build_type.lower())
        os.mkdir(curr_build_dir)
        subprocess.run(['cmake', '..'], cwd=curr_build_dir)
        subprocess.run(['make', '--jobs=' + str(num_threads)], cwd=curr_build_dir)
        executables[build_type] = os.path.join(curr_build_dir, 'bin/genie')

    return executables


def _clean():
    git_root = _get_git_root()

    for build_type in _build_types:
        log.info("cleaning build type '" + build_type + "'")
        curr_build_dir = os.path.join(git_root, _build_dir_prefix + build_type.lower())
        if os.path.isdir(curr_build_dir):
            shutil.rmtree(curr_build_dir)


def _fastq_tests(executables):
    git_root = _get_git_root()

    fastq_folder = os.path.join(git_root, "data/fastq")
    fastq_files = [
        os.path.join(fastq_folder, "fourteen-records.fastq"),
        os.path.join(fastq_folder, "minimal.fastq"),
    ]

    for build_type in executables:
        executable = executables[build_type]
        log.info('executable: {}'.format(executable))
        for fastq_file in fastq_files:
            log.info('file: {}'.format(fastq_file))
            mbg_file = fastq_file + '.mgb'
            recon_fastq_file = mbg_file + '.fastq'
            subprocess.run([
                executable,
                'run',
                '--force',
                '--input-file', fastq_file,
                '--output-file', mbg_file
            ])
            subprocess.run([
                executable,
                'run',
                '--force',
                '--input-file', mbg_file,
                '--output-file', recon_fastq_file
            ])
            subprocess.run([
                'bash',
                os.path.join(git_root, 'conformance_tests/cmp_reordered_fastq.sh'),
                '-1', fastq_file,
                '-2', recon_fastq_file
            ])


def main():
    # Basic log config
    format_string = '[%(asctime)s] [%(filename)25s:%(funcName)25s] [%(levelname)-8s] --- %(message)s'
    log.basicConfig(format=format_string, level=log.INFO)

    # Argument parser
    parser = argparse.ArgumentParser(description='conformance tests')
    parser.add_argument('-l', '--log_level', help='log level', choices=_avail_log_levels.keys(), default='info')
    parser.add_argument('-t', '--num_threads', help='number of threads (all available if not specified)')
    args = parser.parse_args()

    # Log level
    try:
        log_level = _avail_log_levels[args.log_level]
    except KeyError:
        log.warning("invalid log level '%s' (using fall-back log level 'info')", args.log_level)
        log_level = log.INFO
    logger = log.getLogger()
    logger.setLevel(log_level)

    # Number of threads
    if args.num_threads is None:
        args.num_threads = multiprocessing.cpu_count()

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

    # Build Genie and execute tests
    try:
        _clean()
        executables = _build(args.num_threads)
        _fastq_tests(executables)
        _clean()
    except InternalError:
        log.error('internal error')
        log.error('traceback follows')
        raise InternalError


if __name__ == "__main__":
    main()
