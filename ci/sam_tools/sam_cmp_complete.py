#!/usr/bin/env python3
# Compare two fastq files
import argparse
import os
import sys
from shutil import copyfile, move
from sam_sort import sam_sort
from sam_remove_header import sam_remove_header
from sam_cmp import sam_cmp


def sam_cmp_complete(input_first, input_second, skip_qvs, skip_names):
    sam_remove_header(input_first, input_first + "_headerless")
    sam_remove_header(input_second, input_second + "_headerless")
    sam_sort(input_first + "_headerless", input_first + "_sorted")
    sam_sort(input_second + "_headerless", input_second + "_sorted")
    os.remove(input_first + "_headerless")
    os.remove(input_second + "_headerless")
    sam_cmp(input_first + "_sorted", input_second + "_sorted", skip_qvs, skip_names)
    os.remove(input_first + "_sorted")
    os.remove(input_second + "_sorted")

if __name__ == "__main__":
    import argparse

    ap = argparse.ArgumentParser()
    ap.add_argument("-i", "--input_first", required=True, \
                    help="First fastq input file")
    ap.add_argument("-j", "--input_second", required=True, \
                    help="First fastq output file")
    ap.add_argument("-n", "--skip_names", action='store_true', \
                    help="First fastq output file")
    ap.add_argument("-q", "--skip_qvs", action='store_true', \
                    help="First fastq output file")
    args = ap.parse_args()

    sam_cmp_complete(args.input_first, args.input_second, args.skip_qvs, args.skip_names)
