#!/usr/bin/env python3
# Replace read names in fastq with read names from other fastq


def fastq_name_strip (input_path, output_path, strip_names, strip_qvs):
    """
    Removes read names and/or quality values from a fastq
    :param input_path: Path to input fastq file
    :param output_path: Path to output fastq file
    :param strip_names: If True, names will be removed
    :param strip_qvs: If True, quality values will be removed
    """
    seqlength = 0
    with open(output_path, "w") as outfile:
        with open(input_path, "r") as infile:
            linectr = 0
            for line in infile:
                if linectr == 0:
                    if strip_names:
                        outfile.write("@\n")
                    else:
                        outfile.write(line)
                elif linectr == 1:
                    outfile.write(line)
                    seqlength = len(line) - 1
                elif linectr == 2:
                    outfile.write(line)
                else:
                    if strip_qvs:
                        outfile.write("#" * seqlength + "\n")
                    else:
                        outfile.write(line)
                linectr = linectr + 1
                if linectr == 4:
                    linectr = 0


if __name__ == "__main__":
    import argparse

    ap = argparse.ArgumentParser()

    ap.add_argument("-i", "--input", required=True,
                    help="Sequence and quality source")
    ap.add_argument("-n", "--strip_names", required=False,
                    help="Strip names", action='store_true')
    ap.add_argument("-q", "--strip_qualities", required=False,
                    help="Strip qualities", action='store_true')
    ap.add_argument("-o", "--output", required=True,
                    help="Output file")
    args = ap.parse_args()
    fastq_name_strip(args.input, args.output, args.strip_names, args.strip_qualities)
