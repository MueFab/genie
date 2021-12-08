#!/usr/bin/env python3
# Sort sam by read names or position

import subprocess
import os

def sam_remove_header(input_path, output_path):
    """
    Sort a file using the unix sort command
    :param input_path: File to sort
    :param output_path: Where to save the sorted file
    """
    inHeader=True
    with open(input_path, "r") as file_input:
        with open(output_path, "w") as file_output:
            for read in file_input:
                if inHeader == True:
                    if read[0:3] == "@HD" or \
                       read[0:3] == "@SQ" or \
                       read[0:3] == "@RG" or \
                       read[0:3] == "@PG" or \
                       read[0:3] == "@CO":
                        continue
                    else:
                        inHeader=False
                file_output.write(read)

if __name__ == "__main__":
    import argparse

    ap = argparse.ArgumentParser()
    ap.add_argument("-i", "--input", required=True,
                    help="First fastq input file")
    ap.add_argument("-o", "--output", required=True,
                    help="First fastq output file")
    args = ap.parse_args()

    sam_remove_header(args.input, args.output)
