#!/usr/bin/env python3

def fastq_name_patch(input_path, input_names_path, output_path):
    """
    Replace read names in fastq with read names from another fastq.
    Once the first file ends, remaining records from the name input are appended as-is.
    :param input_path: Path to input fastq for sequences and quality values
    :param input_names_path: Path to input fastq for read names
    :param output_path: Path to output fastq
    """
    def read_fastq_record(file):
        return [file.readline() for _ in range(4)]

    with open(output_path, "w") as outfile, \
         open(input_path, "r") as infile, \
         open(input_names_path, "r") as namefile:

        while True:
            seq_block = read_fastq_record(infile)
            name_block = read_fastq_record(namefile)

            # If main file ends, break loop and append rest from namefile
            if not seq_block[0]:
                break

            # If namefile is shorter, just write the seq_block
            if not name_block[0]:
                outfile.writelines(seq_block)
                continue

            # Patch the name
            seq_block[0] = name_block[0]
            outfile.writelines(seq_block)

        # Append remaining records from namefile
        while True:
            name_block = read_fastq_record(namefile)
            if not name_block[0]:
                break
            outfile.writelines(name_block)


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

