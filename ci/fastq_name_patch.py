#!/usr/bin/env python3
#Replace read names in fastq with read names from other fastq

import argparse


ap = argparse.ArgumentParser()

ap.add_argument("-i", "--input", required=True,
   help="Sequence and quality source")
ap.add_argument("-n", "--name_input", required=True,
   help="Name source")
ap.add_argument("-o", "--output", required=True,
   help="Output file")

args = ap.parse_args()

with open(args.output, "w") as outfile:
    with open(args.input, "r") as infile:
        with open(args.name_input, "r") as namefile:
            linectr = 0
            for line in infile:
                if linectr == 0:
                    outfile.write(namefile.readline())
                else:
                    outfile.write(line)
                    namefile.readline()
                linectr = linectr + 1
                if linectr == 4:
                    linectr = 0
