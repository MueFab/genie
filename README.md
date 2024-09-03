<p align="center">
<img src="https://raw.githubusercontent.com/MueFab/genie/docker/data/Genie_Logo.png" style="display:block; width: 70%; margin-left:auto; margin-right:auto" />
</p>

---

[![Build Status](https://dev.azure.com/fabianmuentefering/fabianmuentefering/_apis/build/status/muefab.genie?branchName=develop)](https://dev.azure.com/fabianmuentefering/fabianmuentefering/_build?definitionId=4&view=branches)

# Open Source MPEG-G Codec

## Usage Policy

The open source MPEG-G codec Genie is made available before scientific publication.

This pre-publication software is preliminary and may contain errors.
The software is provided in good faith, but without any express or implied warranties.
We refer the reader to our [license](LICENSE).

The goal of our policy is that early release should enable the progress of science.
We kindly ask to refrain from publishing analyses that were conducted using this software while its development is in progress.

## Mandatory Dependencies

* [OpenMP](https://en.wikipedia.org/wiki/OpenMP) for multithreading (ubuntu: libomp-dev / fedora: libomp-devel)
* [CMake](https://cmake.org) 3.1 or greater (ubuntu: cmake / fedora: cmake)
* [make](https://www.gnu.org/software/make/) (ubuntu: build-essential / fedora: make)
* A compiler compliant to C++11  (ubuntu: build-essential / fedora: gcc)

## Optional Dependencies

* [Doxygen](https://www.doxygen.nl) for building the HTML documentation
* [HTSlib](https://github.com/samtools/htslib) for SAM/BAM file support (ubuntu: libhts-dev / fedora: htslib-devel)
    * GNU [autoconf](https://www.gnu.org/software/autoconf/) (ubuntu: autoconf / fedora: autoconf)
    * GNU [automake](https://www.gnu.org/software/automake/) (ubuntu: automake / fedora: automake)
    * [zlib](https://www.zlib.net/) development files (ubuntu: zlib1g-dev / fedora: zlib-devel)
    * [libbz2](https://gitlab.com/bzip2/bzip2) development files (ubuntu: libbz2-dev / fedora: bzip2-devel)
    * [liblzma](https://tukaani.org/xz/) development files (ubuntu: liblzma-dev / fedora: xz-devel)
    * [libcurl](https://curl.se/) development files  (ubuntu: libcurl4-{gnutls,nss,openssl}-dev / fedora: libcurl-devel)
    * [libcrypto](https://www.openssl.org/docs/man3.0/man7/crypto.html) development files (ubuntu: lib{gnutls,nss3,ssl}-dev / fedora: openssl-devel)

## Quickstart

### Getting and Building Genie

We provide the script `get_genie.sh` to quickly build Genie.
It will automatically clone the Genie repository, build HTSlib as a dependency for SAM file support, and download a few small example files for testing purposes. The script needs git and all dependencies for genie and htslib installed (listed above).

    mkdir genie_buildspace
    cd genie_buildspace
    wget https://raw.githubusercontent.com/MueFab/genie/main/util/get_genie.sh
    bash ./get_genie.sh

The script will work only in the `genie_buildspace` directory.
It will not modify any user or system directories.

Alternatively, you can manually build HTSlib and Genie using CMake.

### Verifying

To check that Genie is working correctly, you can execute the following commands.

Transcoding from FASTQ and SAM into MPEG-G records:

    ./genie transcode-fastq -i ./data/example_1.fastq --input-suppl-file ./data/example_2.fastq -o ./data/transcoded_mgrec/unaligned.mgrec
    ./genie transcode-sam -c -i ./data/example.sam -r ./data/example.fa -o ./data/transcoded_mgrec/aligned.mgrec

Compression using the four encoding processes in Genie (Global Assembly Encoding, Low Latency Encoding, Local Assembly Encoding, Reference-Based Encoding):

    # Unaligned data without low latency flag -> Global Assembly Encoding
    ./genie run -i ./data/transcoded_mgrec/unaligned.mgrec -o ./data/encoded/global_assembly.mgb
    
    # Unaligned data with low latency flag -> Low Latency Encoding
    ./genie run -i ./data/transcoded_mgrec/unaligned.mgrec -o ./data/encoded/low_latency.mgb --low-latency
    
    # Aligned data without any reference specified -> Local Assembly Encoding
    ./genie run -i ./data/transcoded_mgrec/aligned.mgrec -o ./data/encoded/local_assembly.mgb
    
    # Aligned data with reference specified -> Reference Based Encoding
    ./genie run -i ./data/transcoded_mgrec/aligned.mgrec -o ./data/encoded/reference_based.mgb -r ./data/example.fa 

Decompression:

    ./genie run -i ./data/encoded/global_assembly.mgb -o ./data/decoded/global_assembly.mgrec
    ./genie run -i ./data/encoded/low_latency.mgb -o ./data/decoded/low_latency.mgrec
    ./genie run -i ./data/encoded/local_assembly.mgb -o ./data/decoded/local_assembly.mgrec
    ./genie run -i ./data/encoded/reference_based.mgb -o ./data/decoded/reference_based.mgrec -r ./data/example.fa

Transcoding from MPEG-G records back to FASTQ and SAM:

    ./genie transcode-fastq -i ./data/decoded/global_assembly.mgrec -o ./data/transcoded_legacy/global_assembly_1.fastq --output-suppl-file ./data/transcoded_legacy/global_assembly_2.fastq
    ./genie transcode-fastq -i ./data/decoded/low_latency.mgrec -o ./data/transcoded_legacy/low_latency_1.fastq --output-suppl-file ./data/transcoded_legacy/low_latency_2.fastq
    ./genie transcode-sam -i ./data/decoded/local_assembly.mgrec -o ./data/transcoded_legacy/local_assembly.sam -r ./data/example.fa
    ./genie transcode-sam -i ./data/decoded/reference_based.mgrec -o ./data/transcoded_legacy/reference_based.sam -r ./data/example.fa

Compare decoded files to originals (record order might be different, thus they must be sorted before the comparison):
    
    diff <(sort ./data/example_1.fastq) <(sort ./data/transcoded_legacy/global_assembly_1.fastq)
    diff <(sort ./data/example_2.fastq) <(sort ./data/transcoded_legacy/global_assembly_2.fastq)
    diff <(sort ./data/example_1.fastq) <(sort ./data/transcoded_legacy/low_latency_1.fastq)
    diff <(sort ./data/example_2.fastq) <(sort ./data/transcoded_legacy/low_latency_2.fastq)
    
    # First line is removed before comparison, as SAM files store the sorting of records in their header
    diff <(tail -n +2 ./data/example.sam | sort) <(tail -n +2 ./data/transcoded_legacy/reference_based.sam | sort)
    diff <(tail -n +2 ./data/example.sam | sort) <(tail -n +2 ./data/transcoded_legacy/local_assembly.sam | sort)

## Contact

Fabian Müntefering <[muenteferi@tnt.uni-hannover.de](mailto:muenteferi@tnt.uni-hannover.de)>

Mikel Hernaez <[mhernaez@unav.es](mailto:mhernaez@unav.es)>

Jan Voges <[voges@tnt.uni-hannover.de](mailto:voges@tnt.uni-hannover.de)>
