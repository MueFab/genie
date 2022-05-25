#!/usr/bin/env python3
# Sort sam by read names

import subprocess
import os

def sam_cmp(input_first, input_second):
    """
    Sort a file using the unix sort command
    :param input_path: File to sort
    :param output_path: Where to save the sorted file
    """
    cmp_matrix = str("11111111011")
    refTranslate = {}
    with open(input_first, "r") as input_file_first:
        with open(input_second, "r") as input_file_second:
            for line in input_file_first:
                line2 = input_file_second.readline()
                split_1 = line[0:-1].split("\t")
                split_2 = line2[0:-1].split("\t")
                for index in range(11):
                    if cmp_matrix[index] == "0":
                        continue
                    if index == 1:
                        # Flag comparison
                        f1 = int(split_1[index])
                        f2 = int(split_2[index])
                        # Secondary / supplementary alignments encoded the same way
                        sup_1 = ((f1 & 0x800) != 0) or ((f1 & 0x100) != 0)
                        sup_2 = ((f2 & 0x800) != 0) or ((f2 & 0x100) != 0)
                        f1 = f1 & ~0x800
                        f1 = f1 & ~0x100
                        f2 = f2 & ~0x800
                        f2 = f2 & ~0x100

                        # In rare cases, DUP flag can be set only in one segment in SAM, which is not encodable in MPEG-G
                        f2 = f2 & ~0x400
                        f1 = f1 & ~0x400

                        # If split record, no information about reverse complement of pair available
                        if (split_1[2] != split_1[6] and split_1[6] != "=") \
                            or ((split_1[2] == split_1[6] or split_1[6] == "=") and abs(int(split_1[3]) - int(split_1[7])) > 32767):
                            f1 = f1 & ~0x20
                            f2 = f2 & ~0x20

                        # If unpaired record, no information about reverse complement of pair available
                        if ((split_1[7] == split_1[3] and (split_1[6] == "=" or split_1[2] == split_1[6])) or split_1[8] == "0" or split_2[8] == "0"):
                            f1 = f1 & ~0x20
                            f2 = f2 & ~0x20
                        # If unmapped, no information about own reverse complement available
                        if f1 & 0x4:
                            f1 = f1 & ~0x10
                            f2 = f2 & ~0x10
                        if f1 != f2 or sup_1 != sup_2:
                            print("> " + line, end='')
                            print("< " + line2, end='')
                            raise RuntimeError("Cmp error " + str(index))
                        
                    elif index == 2 or index == 6:
                        # Rname is lost in MPEGG part 2, only index left.
                        # However, the index should be used consistently as an alias
                        # So we can build and check a dictionary
                        if split_1[index] in refTranslate:
                            if refTranslate[split_1[index]] != split_2[index]:
                                print("> " + line, end='')
                                print("< " + line2, end='')
                                raise RuntimeError("Cmp error " + str(index))
                        else:
                            refTranslate[split_1[index]] = split_2[index]
                    elif index == 5:
                        if int(split_1[1]) & 0x4:
                            continue # Cigar does not matter with unmapped segments
                        # If = or X was in the original cigar can't be encoded in MPEGG
                        c1 = split_1[index].replace("=", "M").replace("X", "M")
                        c2 = split_2[index].replace("=", "M").replace("X", "M")
                        if c1 != c2:
                            print("> " + line, end='')
                            print("< " + line2, end='')
                            raise RuntimeError("Cmp error " + str(index))#
                    elif index == 7:
                        # If unpaired record, pnext should point to itself
                        if (split_1[7] == split_1[3] and (split_1[6] == "=" or split_1[2] == split_1[6])):
                            continue
                        if split_1[7] != split_2[7]:
                            print("> " + line, end='')
                            print("< " + line2, end='')
                            raise RuntimeError("Cmp error " + str(index))#
                    elif split_1[index] != split_2[index]:
                        print("> " + line, end='')
                        print("< " + line2, end='')
                        raise RuntimeError("Cmp error " + str(index))
            if input_file_second.readline() != "":
                raise RuntimeError("Number of records differ")
                

if __name__ == "__main__":
    import argparse

    ap = argparse.ArgumentParser()
    ap.add_argument("-i", "--input_first", required=True, \
                    help="First sam input file")
    ap.add_argument("-j", "--input_second", required=True, \
                    help="First sam output file")
    args = ap.parse_args()

    sam_cmp(args.input_first, args.input_second)
