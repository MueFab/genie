# Comparing GABAC

The Bash script ``scripts/perform_codec_comparison.sh`` can be used to compare the performance of GABAC to other tools. The scripts compresses and decompresses a test file using gzip, bzip2, xz (implementing the LZMA algorithm), rANS order 0 and rANS order 1 (see https://github.com/voges/rans.git), and gabacify when executing e.g. the following command from the ``scripts``directory:

    ./perform_codec_comparison ../resources/input_files/one_mebibyte_random

The compression and decompression times, the maximum RAM usage, and the compressed file sizes will be logged in the file ``../resources/input_files/one_mebibyte_random.codec_stats``.

**NOTE**: gabacify is designed to run on pieces of data which sizes lie below 1 GB. The entire input file will be read into memory and several buffers will be allocated. The estimated RAM usage for compressing a 1 GB file lies between 2 GB and 4 GB. To reduce RAM usage you can tell gabacify to split the input file into blocks of fixed size (using -b [Blocksize in bytes]). This can reduce compression performance but can be much more ressource friendly. Choose a blocksize divisible by 8 for optimal performance.
