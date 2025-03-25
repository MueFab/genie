<p align="center">
<img src="https://raw.githubusercontent.com/MueFab/genie/develop/data/Genie_Logo.png" style="display:block; width: 70%; margin-left:auto; margin-right:auto" />
</p>

<div align="center">
   
[![Build Status](https://github.com/muefab/genie/actions/workflows/ci.yml/badge.svg?branch=develop)](https://github.com/MueFab/genie/actions?query=branch%3Adevelop)
[![Static Badge](https://img.shields.io/badge/License-BSD%203%E2%80%93Clause-blue?style=flat&link=https%3A%2F%2Fgithub.com%2FMueFab%2Fgenie%2Fblob%2Fmain%2FLICENSE)](https://github.com/MueFab/genie/blob/develop/LICENSE)
[![Static Badge](https://img.shields.io/badge/Paper-Communications%20Biology-orange?style=flat&link=https%3A%2F%2Fwww.nature.com%2Farticles%2Fs42003-024-06249-8)](https://www.nature.com/articles/s42003-024-06249-8)
[![Static Badge](https://img.shields.io/badge/Docker%20Hub-Images-purple?style=flat&logo=docker)](https://hub.docker.com/r/muefab/genie)

</div>

<h1 align="center"> Open Source MPEG-G Codec </h1>

---

## Motivation

With the rapid advancements in DNA sequencing technology, the amount of genomic data has been growing exponentially. Handling this data efficiently has become a major challenge due to the diverse formats and statistical properties involved in its processing. From raw nucleotide sequences stored in FASTQ files to aligned data in BAM format, specialized data compression algorithms have emerged to address the storage requirements through optimized encoding.

However, existing compression solutions often lack interoperability, limiting their broader adoption. Inspired by the success of video and audio coding standards, the Moving Picture Experts Group (MPEG) introduced MPEG-G (ISO/IEC 23092), the first international standard for genomic information representation. While MPEG-G defines a framework for decoding genomic data, no open-source implementations of its encoding processes have been available until now.

Genie is the first open-source tool that produces MPEG-G compliant bitstreams. By integrating existing compression technologies like SPRING and GABAC, along with new encoding processes defined by the MPEG-G standard, Genie enables efficient compression of both unaligned and aligned genomic data. This framework facilitates interoperability while providing room for further innovation in genomic data compression.

Explore Genie and contribute to the future of genomic data storage and processing with the power of MPEG-G!

## Installing Genie

### 1. Using Genie as a Docker Container (Recommended)

Genie is available on Docker Hub, and running it through Docker is the simplest and recommended way to get started.

#### Installation and Usage

To use Genie via Docker, run the following command:

```bash
docker run --rm -v <dir>:/work muefab/genie:latest <cmd>
```

- `<dir>`: The directory on your computer containing the files you want to process with Genie.
- `<cmd>`: The Genie command you want to execute.
- `muefab/genie:latest`: Refers to the latest stable version of Genie. If you want to try out the current preview version, use `muefab/genie:develop`.

#### Example

```bash
docker run --rm -v /path/to/files:/work muefab/genie:latest run -i global_assembly.mgb -o global_assembly.mgrec
```

This command will run Genie in a container and process the files located in `/path/to/files`.

### 2. Building Genie on Bare Metal

See [CONTRIBUTING.md](./CONTRIBUTING.md).

## Verifying

To check that Genie is working correctly, you can execute the following commands.

Compression using the four encoding processes in Genie (Global Assembly Encoding, Low Latency Encoding, Local Assembly Encoding, Reference-Based Encoding):

    # Unaligned data without low latency flag -> Global Assembly Encoding
    genie run -i ./data/transcoded_mgrec/unaligned.fastq -o ./data/encoded/global_assembly.mgb
    
    # Unaligned data with low latency flag -> Low Latency Encoding
    genie run -i ./data/transcoded_mgrec/unaligned.fastq -o ./data/encoded/low_latency.mgb --low-latency
    
    # Aligned data without any reference specified -> Local Assembly Encoding
    genie run -i ./data/transcoded_mgrec/aligned.bam -o ./data/encoded/local_assembly.mgb
    
    # Aligned data with reference specified -> Reference Based Encoding
    genie run -i ./data/transcoded_mgrec/aligned.bam -o ./data/encoded/reference_based.mgb -r ./data/example.fa 

Decompression:

    ./genie run -i ./data/encoded/global_assembly.mgb -o ./data/decoded/global_assembly.fastq
    ./genie run -i ./data/encoded/low_latency.mgb -o ./data/decoded/low_latency.fastq
    ./genie run -i ./data/encoded/local_assembly.mgb -o ./data/decoded/local_assembly.sam
    ./genie run -i ./data/encoded/reference_based.mgb -o ./data/decoded/reference_based.sam -r ./data/example.fa

## Contact

Fabian Müntefering <[muenteferi@tnt.uni-hannover.de](mailto:muenteferi@tnt.uni-hannover.de)>

Mikel Hernaez <[mhernaez@unav.es](mailto:mhernaez@unav.es)>

Jan Voges <[voges@tnt.uni-hannover.de](mailto:voges@tnt.uni-hannover.de)>

## Cite Us

Please cite us if you are using Genie in your work.
You are welcome to choose which resource is most representative of what aspect of Genie you called on in your work.

### Genie

Plain text:

> Müntefering, F., Adhisantoso, Y.G., Chandak, S. et al. Genie: the first open-source ISO/IEC encoder for genomic data. Commun Biol 7, 553 (2024). https://doi.org/10.1038/s42003-024-06249-8

BibTeX:

```
@article{muntefering_genie_2024,
    title = {Genie: the first open-source {ISO}/{IEC} encoder for genomic data},
    author = {Müntefering, Fabian and Adhisantoso, Yeremia Gunawan and Chandak, Shubham and Ostermann, Jörn and Hernaez, Mikel and Voges, Jan},
    journal = {Communications Biology},
    volume = {7},
    number = {1},
    pages = {1--10},
    year = {2024},
    doi = {10.1038/s42003-024-06249-8},
}
```

### MPEG-G

Plain text:

> J. Voges, M. Hernaez, M. Mattavelli and J. Ostermann, "An Introduction to MPEG-G: The First Open ISO/IEC Standard for the Compression and Exchange of Genomic Sequencing Data," in Proceedings of the IEEE, vol. 109, no. 9, pp. 1607-1622, Sept. 2021, doi: 10.1109/JPROC.2021.3082027.

BibTeX:

```
@article{voges_introduction_2021,
    title = {An {Introduction} to {MPEG}-{G}: {The} {First} {Open} {ISO}/{IEC} {Standard} for the {Compression} and {Exchange} of {Genomic} {Sequencing} {Data}},
    author = {Voges, Jan and Hernaez, Mikel and Mattavelli, Marco and Ostermann, Jörn},
    journal = {Proceedings of the IEEE},
    volume = {109},
    number = {9},
    pages = {1607--1622},
    year = {2021},
    doi = {10.1109/JPROC.2021.3082027}
}
```
