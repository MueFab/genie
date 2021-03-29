#!/usr/bin/env python3

import argparse
import os
import sys


def main():
    parser = argparse.ArgumentParser(description='convert from SAM to FASTQ')
    parser.add_argument('sam_file_name', help='SAM file name')
    args = parser.parse_args()

    if not os.path.isfile(args.sam_file_name):
        sys.exit("error: this is not a file: {}".format(args.sam_file_name))

    if not args.sam_file_name.endswith(".sam"):
        sys.exit("error: SAM file name must end with '.sam'")

    with open(args.sam_file_name, 'r') as sam_file:
        while 1:
            line = sam_file.readline()
            if not line:
                break
            if not line.startswith('@'):
                fields = line.split('\t')
                sys.stdout.write("@" + fields[0] + "\n")  # rname
                sys.stdout.write(fields[9] + "\n")  # seq
                sys.stdout.write("+" + "\n")
                sys.stdout.write(fields[10] + "\n")  # qual


if __name__ == "__main__":
    main()
