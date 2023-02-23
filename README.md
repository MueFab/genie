# Genie

Open Source MPEG-G Codec

[![Build Status](https://dev.azure.com/fabianmuentefering/fabianmuentefering/_apis/build/status/muefab.genie?branchName=develop)](https://dev.azure.com/fabianmuentefering/fabianmuentefering/_build?definitionId=4&view=branches)

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
* CMake 3.1 or greater
* A compiler compliant to C++11
* Doxygen in case you want to build the HTML-documentation

## Quickstart

### Building

We provide a script to quickly build and try genie. It will automatically check out the genie repository, build htslib as a dependeny for SAM file support, and download a few small example data for testing purposes.

    mkdir genie_buildspace
    cd genie_buildspace
    wget https://github.com/MueFab/genie/tree/main/util/get_genie.sh
    bash ./get_genie.sh

Alternatively, you can manually build htslib and Genie using cmake.

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

## Who do I talk to?

Jan Voges <[voges@tnt.uni-hannover.de](mailto:voges@tnt.uni-hannover.de)>
Mikel Hernaez <[mhernaez@unav.es](mailto:mhernaez@unav.es)>
Fabian Müntefering <[muenteferi@tnt.uni-hannover.de](mailto:muenteferi@tnt.uni-hannover.de)>


