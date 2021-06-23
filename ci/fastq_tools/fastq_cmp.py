#!/usr/bin/env python3
# Compare two fastq files

def fastq_cmp(input_path_1, input_path_2, skip_names, skip_qualities):
    """
    Compare two fastq files and throw an exception if they are not equal
    :param input_path_1: Fastq input path 1
    :param input_path_2: Fastq input path 2
    :param skip_names: Do not compare names
    :param skip_qualities: Do not compare qualities
    """
    with open(input_path_1, "r") as infile_first:
        with open(input_path_2, "r") as infile_second:
            linectr = 0
            for line in infile_first:
                line2 = infile_second.readline()
                if line != line2:
                    if linectr == 0 and not skip_names or \
                       linectr == 3 and not skip_qualities or \
                       linectr == 1 or \
                       linectr == 2:
                        print ("> " + line, end='')
                        print ("< " + line2, end='')
                        raise RuntimeError("Compare failed")
                linectr = linectr + 1
                if linectr == 4:
                    linectr = 0


if __name__ == "__main__":
    import argparse

    ap = argparse.ArgumentParser()

    ap.add_argument("-i", "--input_first", required=True,
                    help="First fastq")
    ap.add_argument("-j", "--input_second", required=True,
                    help="Second fastq")
    ap.add_argument("-n", "--skip_names", required=False,
                    help="Do not compare names", action='store_true')
    ap.add_argument("-q", "--skip_qualities", required=False,
                    help="Do not compare quality values", action='store_true')
    args = ap.parse_args()

    fastq_cmp(args.input_first, args.input_second, args.skip_names, args.skip_qualities)
