#!/usr/bin/env python3
# Compare two fastq files
import argparse
import subprocess
import os
import sys
from shutil import copyfile, move

bashCommand = "git rev-parse --show-toplevel"
process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
output, error = process.communicate()
if process.returncode:
    print("Error getting git path")
    sys.exit(1)
script_path = output.decode()[:-1] + "/ci"

ap = argparse.ArgumentParser()

ap.add_argument("-i", "--first_pair_first_file", required=True,
   help="First pair, first fastq")
ap.add_argument("-j", "--first_pair_second_file", required=False,
   help="First pair, second fastq")
ap.add_argument("-p", "--second_pair_first_file", required=True,
   help="Second pair, first fastq")
ap.add_argument("-q", "--second_pair_second_file", required=False,
   help="Secons pair, second fastq")
ap.add_argument("-n", "--skip_names", required=False,
   help="Do not compare names", action='store_true')
ap.add_argument("-s", "--skip_qualities", required=False,
   help="Do not compare quality values", action='store_true')
ap.add_argument("-c", "--patched_names", required=False,
   help="Use names from first fastq only", action='store_true')
ap.add_argument("-u", "--unordered_cmp", required=False,
   help="Order of records is not important", action='store_true')
ap.add_argument("-x", "--unordered_cmp_broken_pairs", required=False,
   help="Order of records is not important and pairing is broken", action='store_true')
args = ap.parse_args()


if (args.first_pair_second_file is not None and args.second_pair_second_file is None) or \
   (args.first_pair_second_file is None and args.second_pair_second_file is not None):
    print("Error: paired/unpaired mode not clear")
    sys.exit(1)

if args.skip_names or args.skip_qualities:
    bashCommand = script_path + "/fastq_name_strip.py -i " + args.first_pair_first_file + " -o " + args.first_pair_first_file + ".stripped"
    if args.skip_names:
        bashCommand = bashCommand + " -n"
    if args.skip_qualities:
        bashCommand = bashCommand + " -q"
    print(bashCommand)
    process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
    output, error = process.communicate()
    if process.returncode:
        print("Error stripping")
        sys.exit(1)
    copyfile(args.second_pair_first_file, args.second_pair_first_file + ".stripped")

    if args.first_pair_second_file is not None:
        bashCommand = script_path + "/fastq_name_strip.py -i " + args.first_pair_second_file + " -o " + args.first_pair_second_file + ".stripped"
        if args.skip_names:
            bashCommand = bashCommand + " -n"
        if args.skip_qualities:
            bashCommand = bashCommand + " -q"
        print(bashCommand)
        process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
        output, error = process.communicate()
        if process.returncode:
            print("Error stripping")
            sys.exit(1)
        copyfile(args.second_pair_second_file, args.second_pair_second_file + ".stripped")
else:
    copyfile(args.first_pair_first_file, args.first_pair_first_file + ".stripped")
    copyfile(args.second_pair_first_file, args.second_pair_first_file + ".stripped")
    if args.first_pair_second_file is not None:
        copyfile(args.first_pair_second_file, args.first_pair_second_file + ".stripped")
        copyfile(args.second_pair_second_file, args.second_pair_second_file + ".stripped")


if args.unordered_cmp:
    if args.unordered_cmp_broken_pairs:
        bashCommand = script_path + "/fastq_sort.py -i " + args.first_pair_first_file + ".stripped -o " + args.first_pair_first_file + ".sorted"
        print(bashCommand)
        process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
        output, error = process.communicate()
        if process.returncode:
            print("Error sorting")
            sys.exit(1)
       
        bashCommand = script_path + "/fastq_sort.py -i " + args.second_pair_first_file + ".stripped -o " + args.second_pair_first_file + ".sorted"
        print(bashCommand)
        process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
        output, error = process.communicate()
        if process.returncode:
            print("Error sorting")
            sys.exit(1)

        if args.first_pair_second_file is not None:
            bashCommand = script_path + "/fastq_sort.py -i " + args.first_pair_second_file + ".stripped -o " + args.first_pair_second_file + ".sorted"
            print(bashCommand)
            process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
            output, error = process.communicate()
            if process.returncode:
                print("Error sorting")
                sys.exit(1)
       
            bashCommand = script_path + "/fastq_sort.py -i " + args.second_pair_second_file + ".stripped -o " + args.second_pair_second_file + ".sorted"
            print(bashCommand)
            process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
            output, error = process.communicate()
            if process.returncode:
                print("Error sorting")
                sys.exit(1)
    else:
        bashCommand = script_path + "/fastq_sort.py -i " + args.first_pair_first_file + ".stripped -o " + args.first_pair_first_file + ".sorted"
        if args.first_pair_second_file is not None:
            bashCommand = bashCommand + " -j " + args.first_pair_second_file + ".stripped -p " + args.first_pair_second_file + ".sorted"
        print(bashCommand)
        process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
        output, error = process.communicate()
        if process.returncode:
            print("Error sorting")
            sys.exit(1)

        bashCommand = script_path + "/fastq_sort.py -i " + args.second_pair_first_file + ".stripped -o " + args.second_pair_first_file + ".sorted"
        if args.first_pair_second_file is not None:
            bashCommand = bashCommand + " -j " + args.second_pair_second_file + ".stripped -p " + args.second_pair_second_file + ".sorted"
        print(bashCommand)
        process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
        output, error = process.communicate()
        if process.returncode:
            print("Error sorting")
            sys.exit(1)

else:
    move(args.first_pair_first_file + ".stripped", args.first_pair_first_file + ".sorted")
    move(args.second_pair_first_file + ".stripped", args.second_pair_first_file + ".sorted")
    if args.first_pair_second_file is not None:
        move(args.first_pair_second_file + ".stripped", args.first_pair_second_file + ".sorted")
        move(args.second_pair_second_file + ".stripped", args.second_pair_second_file + ".sorted")

if args.first_pair_second_file is not None:
    if args.patched_names:
        bashCommand = script_path + "/fastq_name_patch.py -i " + args.first_pair_second_file + ".sorted -n " + args.first_pair_first_file + ".sorted -o " + args.first_pair_second_file + ".patched"
        print(bashCommand)
        process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
        output, error = process.communicate()
        if process.returncode:
            print("Error patching")
            sys.exit(1)
        os.remove(args.first_pair_second_file + ".sorted") 
    else:
        move(args.first_pair_second_file + ".sorted", args.first_pair_second_file + ".patched")
    move(args.second_pair_second_file + ".sorted", args.second_pair_second_file + ".patched")
move(args.first_pair_first_file + ".sorted", args.first_pair_first_file + ".patched")
move(args.second_pair_first_file + ".sorted", args.second_pair_first_file + ".patched")


bashCommandBase = script_path + "/fastq_cmp.py"
if args.skip_names:
    bashCommandBase = bashCommandBase + " -n"
if args.skip_qualities:
    bashCommandBase = bashCommandBase + " -q"

bashCommand = bashCommandBase + " -i " + args.first_pair_first_file + ".patched" + " -j " + args.second_pair_first_file + ".patched"
print(bashCommand)
process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
output, error = process.communicate()
if process.returncode:
    print("Error comparing file 1")
    sys.exit(1)
os.remove(args.first_pair_first_file + ".patched") 
os.remove(args.second_pair_first_file + ".patched") 

if args.first_pair_second_file is not None:
    bashCommand = bashCommandBase + " -i " + args.first_pair_second_file + ".patched" + " -j " + args.second_pair_second_file + ".patched"
    print(bashCommand)
    process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
    output, error = process.communicate()
    if process.returncode:
        print("Error comparing file 2")
        sys.exit(1)
    os.remove(args.first_pair_second_file + ".patched") 
    os.remove(args.second_pair_second_file + ".patched") 
