#!/usr/bin/env python3
# Sort paired / unpaired fastq by read names 
import argparse
import subprocess
import os
import sys

ap = argparse.ArgumentParser()

ap.add_argument("-i", "--input_first", required=True,
   help="Sequence and quality source")
ap.add_argument("-j", "--input_second", required=False,
   help="Name source")
ap.add_argument("-o", "--output_first", required=True,
   help="Output file")
ap.add_argument("-p", "--output_second", required=False,
   help="Output file")

args = ap.parse_args()

if args.input_second is not None:
    if args.output_second is not None:
        print("Paired mode")
    else:
        print("Error: Paired input, but no paired output specified")
        sys.exit(1)
else:
    if args.output_second is not None:
        print("Error: Paired output, but no paired input specified")
        sys.exit(1)
    else:
        print("Unpaired mode")

# Step 1: Write all data belonging together in one line, use 0xff as seperator

with open(args.output_first + ".tmp", "w") as outfile:
    with open(args.input_first, "r") as infile_first:
        if args.input_second is not None:
            infile_second = open(args.input_second, "r")
        linectr = 0
        for line in infile_first:  
            outfile.write(line[:-1])
            linectr = linectr + 1
            if linectr == 4:
                if args.input_second is not None:
                    outfile.write("\xff")
                    for i in range(3):
                        outfile.write(infile_second.readline()[:-1] + "\xff")
                    outfile.write(infile_second.readline()[:-1])
                outfile.write("\n")
                linectr = 0
            else:
                outfile.write("\xff")
        if args.input_second is not None:
            infile_second.close()

# Step 2: Use unix sort to do the heavy lifting 

with open(args.output_first + ".tmp.sorted", "w") as outfile:
    bashCommand="sort " + args.output_first + ".tmp"
    print(bashCommand)
    process = subprocess.Popen(bashCommand.split(), stdout=outfile)
    output, error = process.communicate()

os.remove(args.output_first + ".tmp") 

# Step 3: Split the newly sorted lines again

with open(args.output_first + ".tmp.sorted", "r") as infile:
    with open(args.output_first, "w") as outfile_first:
        if args.input_second is not None:
            outfile_second = open(args.output_second, "w")
        for line in infile:
            x = line[:-1].split("\xff")
            for i in range(4):
                outfile_first.write(x[i] + "\n")
            if args.input_second is not None:
                for i in range(4, 8):
                    outfile_second.write(x[i] + "\n")
        if args.input_second is not None:
            outfile_second.close()

os.remove(args.output_first + ".tmp.sorted") 
