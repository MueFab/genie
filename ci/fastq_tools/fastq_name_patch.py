#!/usr/bin/env python3
#

def fastq_name_patch(input_path, input_names_path, output_path):
    """
    Replace read names in fastq with read names from other fastq
    :param input_path: Path to input fastq for sequences and quality values
    :param input_names_path: Path to input fastq for read names
    :param output_path: Path to output fastq
    """
    with open(output_path, "w") as outfile:
        with open(input_path, "r") as infile:
            with open(input_names_path, "r") as namefile:
                linectr = 0
                for line in infile:
                    if linectr == 0:
                        outfile.write(namefile.readline())
                    else:
                        outfile.write(line)
                        namefile.readline()
                    linectr = linectr + 1
                    if linectr == 4:
                        linectr = 0


if __name__ == "__main__":
    import argparse

    ap = argparse.ArgumentParser()

    ap.add_argument("-i", "--input", required=True,
                    help="Path to input fastq for sequences and quality values")
    ap.add_argument("-n", "--name_input", required=True,
                    help="Path to input fastq for read names")
    ap.add_argument("-o", "--output", required=True,
                    help="Path to output fastq")

    args = ap.parse_args()
    fastq_name_patch(args.input, args.name_input, args.output)
