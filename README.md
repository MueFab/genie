# Genie

Open Source MPEG-G Codec

[![Build Status](https://dev.azure.com/fabianmuentefering/fabianmuentefering/_apis/build/status/mitogen.genie?branchName=develop)](https://dev.azure.com/fabianmuentefering/fabianmuentefering/_build?definitionId=3&view=branches)

---

## Usage policy

The open source MPEG-G codec Genie is made available before scientific publication.

This pre-publication software is preliminary and may contain errors.
The software is provided in good faith, but without any express or implied warranties.
We refer the reader to our [license](LICENSE).

The goal of our policy is that early release should enable the progress of science.
We kindly ask to refrain from publishing analyses that were conducted using this software while its development is in progress.

## Dependencies

* OpenMP for multithreading
* htslib (https://github.com/samtools/htslib) for support of SAM files 
* CMake 3.1 or greater
* A compiler compliant to C++11
* Doxygen in case you want to build the HTML-documentation

## Building

Clone the repository:

    git clone https://github.com/mitogen/genie.git

Build all libraries and executables:

    mkdir build
    cd build
    cmake ..
    make

This will generate the Genie application at ``build/bin/genie``.

CMake-Options:

* -DBUILD_COVERAGE=ON: Build coverage 
* -DBUILD_DOCUMENTATION=ON: Build the doxygen documentation
* -DBUILD_TESTS=ON: Build test cases
* -DGENIE_USE_OPENMP=OFF: Deactivate multithreading support (drops OpenMP dependency)
* -DGENIE_SAM_SUPPORT=OFF: Deactivate SAM support (drops htslib dependency)
* -DCMAKE_CXX_COMPILER="<path>": Specify a custom compiler path
    
## Quickstart Fastq
Convert fastq to mgrec:
    
    genie transcode-fastq -i myfile_1.fastq --input-suppl-file  myfile_2.fastq -o myfile.mgrec

Compress mgrec to mgb:
    
    genie run -i myfile.mgrec -o myfile.mgb
    
Decompress mgb to mgrec:
    
    genie run -i myfile.mgb -o myfile_decoded.mgrec
    
Convert mgrec to fastq:
    
    genie transcode-fastq -i myfile_decoded.mgrec  -o myfile_decoded_1.fastq --output-suppl-file  myfile_decoded_2.fastq
    
## Quickstart Sam
Convert sam to mgrec:
    
    genie transcode-sam -i myfile.sam -o myfile.mgrec --ref myref.fasta  # myfile.sam must be sorted by read-ID!
    
Compress mgrec to mgb:
    
    genie run -i myfile.mgrec -o myfile.mgb
    
Decompress mgb to mgrec:
    
    genie run -i myfile.mgb -o myfile_decoded.mgrec
    
Convert mgrec to sam:
    
    genie transcode-sam -i myfile.mgrec -o myfile.sam
    
    
## Genie Operations
    
The genie applications contains several operations which can be selected with the first command line option:
    
    genie <operation name> <operation specific arguments>
    
Following operations are available:

* help: Display information about available operations
* run: Compress / Decompress between an MPEG-G compressed mgb-file and an uncompressed MPEG-G mgrec-file
* gabac: Entropy encode a block of data
* transcode-fastq: Convert between fastq and mgrec files
* transcode-sam: Convert between sam and mgrec files
    
## Genie run
The run operation performs compression from the mgrec file format (uncompressed MPEG-G records) into the mgb file format (compressed MPEG-G records) and vice versa. If a compression or decompression shall be performed is recognized by the file extensions ".mgb" and ".mgrec" provided to the -i and -o arguments.
    
Following CLI-Arguments are available:
* --help / -h: Display information about CLI-Arguments
* --input-file / -i (required): Input file path (mgb or mgrec).
* --output-file / -o (required): Output file path (mgb or mgrec).
* --threads / -t: Number of threads to use.
* --force / -f: Flag, if set already existing output files are overridden.
* --input-ref-file: Path to a reference fasta file. Only relevant for aligned records. If no path is provided, a computed reference will be used instead.
* --working-dir / -w: Path to a directory where temporary files can be stored. If no path is provided, the current working dir is used. Please make sure that enough space is available.
* --qv: How to encode quality values. Possible values are "lossless" (default, keep all values), "calq" (quantize values with calq) and "none" (discard all values).
* --read-ids: How to encode read ids. Possible values are "lossless" (default, keep all values) and "none" (discard all values).
* --low-latency: Flag, if set no global reference will be calculated for unaligned records. This will increase encoding speed, but decrease compression rate.
* --combine-pairs: Flag, if provided to a decoding operation, unaligned reads will get matched to their mate again. Note: has no effect if encoded with --low-latency in case of aligned reads only. Does not work if encoded with --read-ids "none"
    
## Genie transcode-fastq
Converts between fastq records and uncompressed MPEG-G records (mgrec). The direction of the conversion is recognized by the file extensions provided via the input and output file names.
    
Following CLI-Arguments are available:
* --help / -h: Display information about CLI-Arguments
* --input-file / -i (required): Input file path (mgb or mgrec).
* --output-file / -o (required): Output file path (mgb or mgrec).
* --threads / -t: Number of threads to use.
* --force / -f: Flag, if set already existing output files are overridden.
* --input-suppl-file: Path to second input fastq file in paired mode.
* --output-suppl-file: Path to second output fastq file in paired mode.

## Genie transcode-sam
Converts between sam records and uncompressed MPEG-G records (mgrec). The direction of the conversion is recognized by the file extensions provided via the input and output file names.
    
Following CLI-Arguments are available:
* --help / -h: Display information about CLI-Arguments
* --input-file / -i (required): Input file path (mgb or mgrec).
* --output-file / -o (required): Output file path (mgb or mgrec).
* --threads / -t: Number of threads to use.
* --force / -f: Flag, if set already existing output files are overridden.
* --working-dir / -w: Path to a directory where temporary files can be stored. If no path is provided, the current working dir is used. Please make sure, that enough space is available.
* --ref: Path to reference fasta file.
    
## Who do I talk to?

Jan Voges <[voges@tnt.uni-hannover.de](mailto:voges@tnt.uni-hannover.de)>

Mikel Hernaez <[mhernaez@illinois.edu](mailto:mhernaez@illinois.edu)>


