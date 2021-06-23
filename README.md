# Genie

Open Source MPEG-G Codec

[![Build Status](https://dev.azure.com/janvoges/genie/_apis/build/status/mitogen.genie?branchName=master)](https://dev.azure.com/janvoges/genie/_build/latest?definitionId=1&branchName=master)

---

## Usage policy

The open source MPEG-G codec Genie is made available before scientific publication.

This pre-publication software is preliminary and may contain errors.
The software is provided in good faith, but without any express or implied warranties.
We refer the reader to our [license](LICENSE).

The goal of our policy is that early release should enable the progress of science.
We kindly ask to refrain from publishing analyses that were conducted using this software while its development is in progress.

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

## Version numbers

We use the [Semantic Versioning 2.0.0](https://semver.org).

That means:

- The release version number format is: MAJOR.MINOR.PATCH
- We increment the
  - MAJOR version when making incompatible API changes,
  - MINOR version when adding functionality in a backwards-compatible manner, and
  - PATCH version when making backwards-compatible bug fixes.
- Pre-release versions are denoted by appending a hyphen and a series of dot separated identifiers immediately following the patch version.
  - Example 1: 1.0.0-alpha.1 ("alpha version 1 of the planned major release 1")
  - Example 2: 1.0.0-beta.1 ("beta version 1 of the planned major release 1")
  - Example 3: 1.0.0-rc.1 ("release candidate (rc) 1 of the planned major release 1")
  - Note: 1.0.0-alpha.1 < 1.0.0-beta.1 < 1.0.0-rc.1 by definition

## Who do I talk to?

Jan Voges <[voges@tnt.uni-hannover.de](mailto:voges@tnt.uni-hannover.de)>

Mikel Hernaez <[mhernaez@illinois.edu](mailto:mhernaez@illinois.edu)>
