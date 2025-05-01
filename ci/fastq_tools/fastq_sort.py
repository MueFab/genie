#!/usr/bin/env python3
# Sort paired / unpaired fastq by read names

import subprocess
import os

def _fastq_sort_read_fastq_record(file):
    """
    Read one fastq record (4 lines) and merge the lines using 0xff as a delimiter
    :param file: Input file object
    :return: Record as single line
    """
    ret = ""
    for i in range(3):
        read = file.readline()
        if len(read) == 0:
            if i == 0:
                return ""
            else:
                raise RuntimeError("Truncated fastq record")
        ret += read[:-1] + "\xff"
    ret += file.readline()
    return ret


def _fastq_sort_merge_single_end(input_path, output_path):
    """
    Copy merged records to output file
    :param input_path: Fastq input file
    :param output_path: Merged tmp output path
    """
    with open(input_path, "r") as file_input:
        with open(output_path, "w") as file_output:
            while True:
                read = _fastq_sort_read_fastq_record(file_input)
                if len(read) == 0:
                    break
                file_output.write(read)


def _fastq_sort_split_single_end(input_path, output_path):
    """
    Split merged records to output file
    :param input_path: Merged tmp input file
    :param output_path: Fastq output file
    """
    with open(input_path, "r") as file_input:
        with open(output_path, "w") as file_output:
            for read in file_input:
                read = read.split(sep="\xff")
                if len(read) != 4:
                    raise RuntimeError("Truncated single fastq record " + str(len(read)))
                read[3] = read[3][:-1]
                for r in read:
                    file_output.write(r + "\n")


def _fastq_sort_merge_paired_end(input_path1, input_path2, output_path):
    """
    Merge paired records into single lines separated by a 0xff delimiter
    Handles unequal file lengths by writing leftover records as singletons.
    :param input_path1: Path of input fastq 1
    :param input_path2: Path of input fastq 2
    :param output_path: Path of output tmp file
    """
    with open(input_path1, "r") as file1, open(input_path2, "r") as file2, open(output_path, "w") as out:
        while True:
            read1 = _fastq_sort_read_fastq_record(file1)
            read2 = _fastq_sort_read_fastq_record(file2)
            if not read1 and not read2:
                break
            # if both present, write combined
            if read1 and read2:
                out.write(read1[:-1] + "\xff" + read2)
            else:
                # write leftover single-end records
                if read1:
                    out.write(read1)
                if read2:
                    out.write(read2)
                # continue writing remaining in whichever file
                source = file1 if read1 else file2
                while True:
                    rem = _fastq_sort_read_fastq_record(source)
                    if not rem:
                        break
                    out.write(rem)
                break


def _fastq_sort_split_paired_end(input_path, output_path1, output_path2):
    """
    Split merged records to output file
    Handles both combined pairs and singleton records.
    :param input_path: Merged tmp input file
    :param output_path1: Fastq output file 1
    :param output_path2: Fastq output file 2
    """
    with open(input_path, "r") as inp, open(output_path1, "w") as out1, open(output_path2, "w") as out2:
        for line in inp:
            parts = line.rstrip("\n").split("\xff")
            if len(parts) == 4:
                # singleton: only write to first
                for p in parts:
                    out1.write(p + "\n")
            elif len(parts) == 8:
                # proper pair
                for p in parts[:4]:
                    out1.write(p + "\n")
                for p in parts[4:]:
                    out2.write(p + "\n")
            else:
                raise RuntimeError(f"Unexpected merged record length: {len(parts)} fields")


def _fastq_sort_unix_sort(input_path, output_path):
    """
    Sort a file using the unix sort command
    :param input_path: File to sort
    :param output_path: Where to save the sorted file
    """
    with open(output_path, "w") as file_out:
        bash_command = "sort " + input_path
        print(bash_command)
        process = subprocess.Popen(bash_command.split(), stdout=file_out)
        process.communicate()
        if process.returncode:
            raise RuntimeError(bash_command + " failed")


def _fastq_sort_paired_end(input_paths, output_paths):
    """
    Sort paired fastq data
    :param input_paths: Tuple (fastq1_path, fastq2_path) for input fastq files
    :param output_paths: Tuple (fastq1_path, fastq2_path) for sorted output fastq files
    """
    _fastq_sort_merge_paired_end(input_paths[0], input_paths[1], output_paths[0] + ".tmp")
    _fastq_sort_unix_sort(output_paths[0] + ".tmp", output_paths[0] + ".tmp.sorted")
    os.remove(output_paths[0] + ".tmp")
    _fastq_sort_split_paired_end(output_paths[0] + ".tmp.sorted", output_paths[0], output_paths[1])
    os.remove(output_paths[0] + ".tmp.sorted")


def _fastq_sort_single_end(input_path, output_path):
    """
    Sort unpaired fastq data
    :param input_path: Input fastq file path
    :param output_path: Output fastq file path
    """
    _fastq_sort_merge_single_end(input_path, output_path + ".tmp")
    _fastq_sort_unix_sort(output_path + ".tmp", output_path + ".tmp.sorted")
    os.remove(output_path + ".tmp")
    _fastq_sort_split_single_end(output_path + ".tmp.sorted", output_path)
    os.remove(output_path + ".tmp.sorted")


def fastq_sort(input_paths, output_paths, independent_sort):
    """
    Sort a fastq file or fastq pair by read names / sequence (if read names equal or not available)
    :param output_paths: Tuple of fastq output paths, paired path may be None
    :param input_paths: Tuple of fastq input paths, paired path may be None
    :param independent_sort: If True, the two files of a fastq file will be sorted independently instead of as a pair
    """
    if (input_paths[1] is None and output_paths[1] is not None) or \
            (input_paths[1] is not None and output_paths[1] is None):
        raise RuntimeError("Number of input files and output files does not match.")

    if output_paths[1] is not None:
        if independent_sort:
            _fastq_sort_single_end(input_paths[0], output_paths[0])
            _fastq_sort_single_end(input_paths[1], output_paths[1])
        else:
            _fastq_sort_paired_end(input_paths, output_paths)
    else:
        _fastq_sort_single_end(input_paths[0], output_paths[0])


if __name__ == "__main__":
    import argparse

    ap = argparse.ArgumentParser()
    ap.add_argument("-i", "--input_first", required=True,
                    help="First fastq input file")
    ap.add_argument("-j", "--input_second", required=False,
                    help="Second fastq input file (if paired)")
    ap.add_argument("-o", "--output_first", required=True,
                    help="First fastq output file")
    ap.add_argument("-p", "--output_second", required=False,
                    help="Second fastq output file (if paired)")
    ap.add_argument("-x", "--independent_sort", required=False,
                    help="Ignore pairing and sort both input files (if two are provided) independently",
                    action='store_true')
    args = ap.parse_args()

    fastq_sort((args.input_first, args.input_second), (args.output_first, args.output_second), args.independent_sort)
