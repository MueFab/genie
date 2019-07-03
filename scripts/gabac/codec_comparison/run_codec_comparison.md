# Run Codec Comparison

This document explains how to compare GABAC to competing codecs.

## Step 1: running GABAC and competing codecs

Given a test file named ``test``, roundtrips with ``gabacify`` (i.e., GABAC) and competing codecs can be performed by running the following command:

    $ ./run_codecs.sh test

This produces the following files:
* ``test.codec_stats.txt``
* ``test.gabac_configuration.json``
* ``test.gabac-a.enc.stdout``
* ``test.gabac-a.enc.stderr``
* ``test.gabac-a.dec.stdout``
* ``test.gabac-a.dec.stderr``
* ``test.gabac.enc.stdout``
* ``test.gabac.enc.stderr``
* ``test.gabac.dec.stdout``
* ``test.gabac.dec.stderr``

The file ``test.codec_stats.txt`` contains the performance statistics per codec, i.e.:
* compression ratio,
* compression/decompression time,
* compression/decompression memory usage.

A sample ``test.codec_stats.txt`` file looks like the following:

      codec  uncompressed_size  compressed_size  compression_time_(s)  compression_max_RSS_(kB)  decompression_time_(s)  decompression_max_RSS_(kB)  
       gzip            4658302          3370324                  0.61                      1800                    0.14                        1616
      bzip2            4658302          2845168                  0.81                      9828                    0.39                        6828
         xz            4658302          2900708                  1.76                    112208                    0.38                        8448
      rans0            4658302          3315668                  0.16                      4716                    0.15                        4796
      rans1            4658302          2797875                  0.17                      5892                    0.18                        6136
    gabac-a            4658302          2853705               1081.35                    251276                    0.71                      112384
      gabac            4658302          2853705                  0.93                    116656                    0.76                      110848

## Step 2: conversion of statistics files to the CSV format

``*.codec_stats.txt`` files must be converted to the CSV format prior to further processing. Assuming that multiple ``*.codec_stats.txt`` files are in the directory ``codec_stats_dir`` the conversion can be done with the following command:

    $ python3 wsv2csv.py codec_stats_dir/*

## Step 3: statistics augmentation and summary

Prior to further processing all CSV file names must be (*manually*) prepended with a two-letter test set identifier, for example:

    01_test0.codec_stats.txt.csv
    01_test1.codec_stats.txt.csv
    02_test0.codec_stats.txt.csv
    02_test1.codec_stats.txt.csv

Statistics can then be summarized in the file ``summary.csv`` with the following command:

    $ python3 augment_codec_stats.py codec_stats_dir/*.csv --output_file summary.csv

Braces () need to be removed from the ``summary.csv`` file to not confuse the R code in the Jupyter notebook from step 4. This can be done with the following commands:

    # Linux
    $ sed -i 's/(//g' summary.csv
    $ sed -i 's/)//g' summary.csv

    # macOS
    $ sed -i "" 's/(//g' summary.csv
    $ sed -i "" 's/)//g' summary.csv

Finally, usually the GABAC analyze runs do not go into our plots. The corresponding rows can be purged with the following command:

    # Linux
    $ sed -i '/gabac-a/d' summary.csv

    # macOS
    $ sed -i "" '/gabac-a/d' summary.csv

## Step 4: plotting of results

The file ``summary.csv`` can be loaded into the Jupyter notebook ``plots.ipynb`` to generate various plots.
