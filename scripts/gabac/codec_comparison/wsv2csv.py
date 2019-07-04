#!/usr/bin/env python3


import argparse
import glob
import logging as log
import os
import re
import sys


def main():
    log.basicConfig(
        format='[%(filename)s] [%(asctime)s] [%(levelname)s] %(message)s',
        level=log.INFO)

    parser = argparse.ArgumentParser(
        description='Convert whitespace separated value (WSV) file(s) to comma'
                    'separated value (CSV) file(s)',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        'path',
        nargs='+',
        help='Path of a file or a folder of files')
    args = parser.parse_args()

    abs_paths = [os.path.join(os.getcwd(), path) for path in args.path]
    if not len(abs_paths):
        log.error("No valid path(s) provided")
        sys.exit(1)
    input_files = set()
    for path in abs_paths:
        if os.path.isfile(path):
            input_files.add(path)

    for input_file in input_files:
        output_file = input_file + ".csv"
        if os.path.isfile(output_file):
            log.warning(
                "Output file '{}' already exists (skipping conversion of '{}')"
                .format(output_file, input_file))
            continue
        log.info("{} -> {}".format(input_file, output_file))
        with open(input_file) as ifp:
            with open(output_file, "w") as ofp:
                line = ifp.readline()
                while line:
                    line = line.strip()
                    line = re.sub("\\s+", ",", line)
                    print("{}".format(line), file=ofp)
                    line = ifp.readline()


if __name__ == '__main__':
    main()
