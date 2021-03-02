#!/usr/bin/env python3
#Replace read names in fastq with read names from other fastq

import argparse


ap = argparse.ArgumentParser()

ap.add_argument("-i", "--input", required=True,
   help="Sequence and quality source")
ap.add_argument("-n", "--strip_names", required=False,
   help="Strip names", action='store_true')
ap.add_argument("-q", "--strip_qualities", required=False,
   help="Strip qualities", action='store_true')
ap.add_argument("-o", "--output", required=True,
   help="Output file")

args = ap.parse_args()

seqlength = 0

with open(args.output, "w") as outfile:
    with open(args.input, "r") as infile:
        linectr = 0
        for line in infile:
            if linectr == 0:
                if args.strip_names:
                     outfile.write("@\n")
                else:
                     outfile.write(line)
            elif linectr == 1:
                outfile.write(line)
                seqlength = len(line) - 1
            elif linectr == 2:
                outfile.write(line)
            else:
                if args.strip_qualities:
                    outfile.write("#" * seqlength + "\n")
            linectr = linectr + 1
            if linectr == 4:
                linectr = 0
