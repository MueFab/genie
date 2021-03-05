#!/usr/bin/env python3
# Compare two fastq files
import argparse
import os
import sys
from shutil import copyfile, move
from fastq_name_strip import fastq_name_strip 
from fastq_cmp import fastq_cmp 
from fastq_sort import fastq_sort 
from fastq_name_patch import fastq_name_patch


def fastq_cmp_complete(primary_files, paired_files, patched_names, broken_names, broken_qualities, broken_order,
                       broken_pairing):
    """
    Compare two paired/unpaired fastq sets. Some errors can be set to be tolerated using the parameters.
    :param primary_files: Tuple of primary fastq paths
    :param paired_files: Tuple of paired fastq paths
    :param patched_names: True if the names of the paired fastq can be replaced by the names of the primary fastq
    :param broken_names: True if names can be unequal
    :param broken_qualities: True if qualities can be unequal
    :param broken_order: True if order of record in the files can be different, but pairing is preserved
    :param broken_pairing: True if pairing information may be lost
    :return:
    """
    if broken_pairing:
        broken_order = True

    # In case that broken qualities or names are expected, remove them
    for i in range(2):
        fastq_name_strip(primary_files[i], primary_files[i] + ".stripped", broken_names, broken_qualities)
        if paired_files[0] is not None:
            fastq_name_strip(paired_files[i], paired_files[i] + ".stripped", broken_names, broken_qualities)

    # In case of broken order, reorder
    if broken_order:
        for i in range(2):
            in_tuple = (primary_files[i] + ".stripped", None)
            out_tuple = (primary_files[i] + ".sorted", None)
            if paired_files[i] is not None:
                in_tuple = (in_tuple[0], paired_files[i] + ".stripped")
                out_tuple = (out_tuple[0], paired_files[i] + ".sorted")
            fastq_sort(in_tuple, out_tuple, broken_pairing)
            os.remove(primary_files[i] + ".stripped")
            if paired_files[i] is not None:
                os.remove(paired_files[i] + ".stripped")
    # In case of correct order, just move the files as they are
    else:
        for i in range(2):
            move(primary_files[i] + ".stripped", primary_files[i] + ".sorted")
            if paired_files[i] is not None:
                move(paired_files[i] + ".stripped", paired_files[i] + ".sorted")

    # In case of replaced names, patch the name in ground truth
    if patched_names and paired_files[0] is not None:
        fastq_name_patch(paired_files[0] + ".sorted", primary_files[0] + ".sorted", paired_files[0] + ".patched")
        os.remove(paired_files[0] + ".sorted")
        move(primary_files[0] + ".sorted", primary_files[0] + ".patched")
        move(primary_files[1] + ".sorted", primary_files[1] + ".patched")
        move(paired_files[1] + ".sorted", paired_files[1] + ".patched")
    # No patched names: Just copy
    else:
        for i in range(2):
            move(primary_files[i] + ".sorted", primary_files[i] + ".patched")
            if paired_files[i] is not None:
                move(paired_files[i] + ".sorted", paired_files[i] + ".patched")

    # Do the comparison
    fastq_cmp(primary_files[0] + ".patched", primary_files[1] + ".patched", broken_names, broken_qualities)
    if paired_files[0] is not None:
        fastq_cmp(paired_files[0] + ".patched", paired_files[1] + ".patched", broken_names, broken_qualities)

    # Remove temporaries
    for i in range(2):
        os.remove(primary_files[i] + ".patched")
        if paired_files[i] is not None:
            os.remove(paired_files[i] + ".patched")


if __name__ == "__main__":
    import argparse

    ap = argparse.ArgumentParser()

    ap.add_argument("-i", "--first_pair_first_file", required=True,
                    help="First pair, first fastq")
    ap.add_argument("-j", "--first_pair_second_file", required=False,
                    help="First pair, second fastq")
    ap.add_argument("-p", "--second_pair_first_file", required=True,
                    help="Second pair, first fastq")
    ap.add_argument("-q", "--second_pair_second_file", required=False,
                    help="Secons pair, second fastq")
    ap.add_argument("-n", "--broken_names", required=False,
                    help="Do not compare names", action='store_true')
    ap.add_argument("-s", "--broken_qualities", required=False,
                    help="Do not compare quality values", action='store_true')
    ap.add_argument("-c", "--patched_names", required=False,
                    help="Use names from first fastq only", action='store_true')
    ap.add_argument("-u", "--broken_order", required=False,
                    help="Order of records is not important", action='store_true')
    ap.add_argument("-x", "--broken_pairing", required=False,
                    help="Order of records is not important and pairing is broken", action='store_true')
    args = ap.parse_args()

    if (args.first_pair_second_file is not None and args.second_pair_second_file is None) or \
            (args.first_pair_second_file is None and args.second_pair_second_file is not None):
        raise RuntimeError("Error: paired/unpaired mode not clear")

    fastq_cmp_complete((args.first_pair_first_file, args.second_pair_first_file),
                       (args.first_pair_second_file, args.second_pair_second_file), args.patched_names,
                       args.broken_names, args.broken_qualities, args.broken_order, args.broken_pairing)
