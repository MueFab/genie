#!/usr/bin/env python3


import argparse
import glob
import logging as log
import os
import pandas as pd
import sys


def main():
    log.basicConfig(
        format='[%(filename)s] [%(asctime)s] [%(levelname)s] %(message)s',
        level=log.INFO)

    parser = argparse.ArgumentParser(
        description='Augment codec statistics',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        'path',
        nargs='+',
        help='Path of a file or a folder of files')
    parser.add_argument(
        '-o',
        '--output_file',
        required=True,
        help='Output file')
    args = parser.parse_args()

    abs_paths = [os.path.join(os.getcwd(), path) for path in args.path]
    if not len(abs_paths):
        log.error("No valid path(s) provided")
        sys.exit(1)
    abs_input_file_paths = set()
    for path in abs_paths:
        if os.path.isfile(path):
            abs_input_file_paths.add(path)

    output_file_path = args.output_file
    if os.path.isfile(output_file_path):
        log.error("Output file '{}' already exists".format(output_file_path))
        sys.exit(1)

    with open(output_file_path, "w") as ofp:
        cnt = 0
        for abs_input_file_path in abs_input_file_paths:
            log.info("{}".format(abs_input_file_path))
            df = pd.read_csv(abs_input_file_path)

            input_file_path = os.path.basename(abs_input_file_path)
            test_file_path = os.path.splitext(abs_input_file_path)[0]
            test_file_path = os.path.splitext(test_file_path)[0]

            df['filename'] = [os.path.basename(test_file_path)] * df.shape[0]
            df['test_set'] = df['filename'].str[0:2]
            df['compression_speed_(MiB/s)'] = (
                (df['uncompressed_size'] / (1024 * 1024)) /
                df['compression_time_(s)'])
            df['decompression_speed_(MiB/s)'] = (
                (df['compressed_size'] / (1024 * 1024)) /
                df['decompression_time_(s)'])
            df['compression_rank'] = (
                df['compressed_size'].rank(method='dense').astype(int))
            df['compression_speed_rank'] = (
                df['compression_speed_(MiB/s)']
                .rank(method='dense', ascending=False).astype(int))
            df['decompression_speed_rank'] = (
                df['decompression_speed_(MiB/s)']
                .rank(method='dense', ascending=False).astype(int))

            if cnt == 0:
                print("{}".format(
                    df.to_csv(index=False, header=True)).strip(), file=ofp)
            else:
                print("{}".format(
                    df.to_csv(index=False, header=False)).strip(), file=ofp)
            cnt += 1

    log.info("Wrote augmented codec statistics to: {}"
             .format(output_file_path))


if __name__ == '__main__':
    main()
