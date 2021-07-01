#!/usr/bin/env python3
# Sort sam by position

import subprocess
import os

def _sam_unix_sort(input_path, output_path):
    """
    Sort a file using the unix sort command
    :param input_path: File to sort
    :param output_path: Where to save the sorted file
    """
    with open(output_path, "w") as file_out:
        my_env = os.environ.copy()
        my_env["LC_COLLATE"] = "C"
        bash_command = "sort -u " + input_path
        print(bash_command)
        process = subprocess.Popen(bash_command.split(), env=my_env, stdout=file_out)
        process.communicate()
        if process.returncode:
            raise RuntimeError(bash_command + " failed")

_permute_order = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

def _sam_permute(input_path, output_path):
    """
    Sort a file using the unix sort command
    :param input_path: File to sort
    :param output_path: Where to save the sorted file
    """
    with open(input_path, "r") as file_input:
        with open(output_path, "w") as file_out:
            for read in file_input:
                splitted = read[0:-1].split("\t")
                for p in range(len(_permute_order)):
                    if p != 0:
                        file_out.write("\xff")
                    file_out.write(splitted[_permute_order[p]])
                file_out.write("\n")
                        

def _sam_revert(input_path, output_path, _permute_order_reverse):
    """
    Sort a file using the unix sort command
    :param input_path: File to sort
    :param output_path: Where to save the sorted file
    """
    with open(input_path, "r") as file_input:
        with open(output_path, "w") as file_out:
            for read in file_input:
                splitted = read[0:-1].split("\xff")
                for p in range(len(_permute_order_reverse)):
                    if p != 0:
                        file_out.write("\t")
                    file_out.write(splitted[_permute_order_reverse[p]])
                file_out.write("\n")

def sam_sort(input_path, output_path):
    _permute_order_reverse = []
    for i in range(len(_permute_order)):
       for j in range(len(_permute_order)):
           if _permute_order[j] == i:
               _permute_order_reverse.append(j)
               continue 

    _sam_permute(input_path, input_path + "_pos")
    _sam_unix_sort(input_path + "_pos", input_path + "_inter_sorted")
    os.remove(input_path + "_pos")
    _sam_revert(input_path + "_inter_sorted", output_path, _permute_order_reverse)
    os.remove(input_path + "_inter_sorted")


if __name__ == "__main__":
    import argparse

    ap = argparse.ArgumentParser()
    ap.add_argument("-i", "--input", required=True,
                    help="First fastq input file")
    ap.add_argument("-o", "--output", required=True,
                    help="First fastq output file")
    args = ap.parse_args()
    sam_sort(args.input, args.output)
    
