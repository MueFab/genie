#!/usr/bin/env python3
# Compare two fastq files
import argparse
import subprocess
import os
import sys

ap = argparse.ArgumentParser()

ap.add_argument("-i", "--input_first", required=True,
   help="First fastq")
ap.add_argument("-j", "--input_second", required=True,
   help="Second fastq")
ap.add_argument("-n", "--skip_names", required=False,
   help="Do not compare names", action='store_true')
ap.add_argument("-q", "--skip_qualities", required=False,
   help="Do not compare quality values", action='store_true')
args = ap.parse_args()


with open(args.input_first, "r") as infile_first:
    with open(args.input_second, "r") as infile_second:
        linectr = 0
        for line in infile_first:
            line2 = infile_second.readline()
            if line != line2:
                if linectr == 0 and not args.skip_names or \
                   linectr == 3 and not args.skip_qualities or \
                   linectr == 1 or \
                   linectr == 2:
                    print ("> " + line, end='')
                    print ("< " + line2, end='')
                    sys.exit(1)
            linectr = linectr + 1
            if linectr == 4:
                linectr = 0
