# Genie

Open Source MPEG-G Codec

**First off, we refer the reader to our [usage policy](USAGE_POLICY.md).**

[![Build Status](https://dev.azure.com/janvoges/genie/_apis/build/status/mitogen.genie?branchName=master)](https://dev.azure.com/janvoges/genie/_build/latest?definitionId=1&branchName=master)

---

## Quick start

Clone the repository:

    git clone https://github.com/mitogen/genie.git

Build all libraries and executables:

    mkdir build
    cd build
    cmake ..
    make

This will generate the Genie application at ``cmake-build/bin/genie``.

See the help for a full range of options: ``genie --help``

Notes:

- CMake 3.1 or greater is required.
- A compiler compliant to C++11 is required.
- A specific C++ compiler can be used by setting the CMake variable ``CMAKE_CXX_COMPILER``.

## Compressing and decompressing FASTQ data

The first step is to generate a set of configuration files:

    genie input_1.fastq [input_2.fastq] --generate --config-dir /path/to/config/dir/

Note the ``--generate`` flag. The configuration files will be stored in the directory ``/path/to/config/dir/``.

The second step is to compress the FASTQ data:

    genie input_1.fastq [input_2.fastq] --config-dir /path/to/config/dir/ --output-file compressed.genie

Note the absence of the ``--generate`` flag.

To uncompress a ``.genie`` file, run the following command:

    genie compressed.genie --config-dir /path/to/config/dir/

## Who do I talk to?

Jan Voges <[voges@tnt.uni-hannover.de](mailto:voges@tnt.uni-hannover.de)>

Mikel Hernaez <[mhernaez@illinois.edu](mailto:mhernaez@illinois.edu)>
