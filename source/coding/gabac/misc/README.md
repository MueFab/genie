# GABAC

Genomics-oriented Context-Adaptive Binary Arithmetic Coding

[![Build Status](https://travis-ci.org/mitogen/gabac.svg?branch=master)](https://travis-ci.org/mitogen/gabac)

**First off, we refer the reader to ``USAGE_POLICY.md``.**

---

## Quick start on Linux

Build the GABAC library and the gabacify application:

    mkdir build
    cd build
    cmake ..
    make gabacify

(See also the ``.travis.yml``.)

For quick build without using any globally installed libraries, see build.sh. 
Make sure you have a recent version of gcc installed (7.3 and newer has been tested).

Perform a test roundtrip from the ``build`` directory:

    $ ./bin/gabacify analyze -i ../resources/input_files/one_mebibyte_random -o config.json  # Generate a configuration for this kind of data

    $ ./bin/gabacify encode -i ../resources/input_files/one_mebibyte_random -c config.json -o one_mebibyte_random.gabac
    $ ./bin/gabacify decode -o one_mebibyte_random_decompressed -c config.json -i one_mebibyte_random.gabac

    $ diff ../resources/input_files/one_mebibyte_random ../resources/input_files/one_mebibyte_random.gabac_uncompressed

## Comparing GABAC to other codecs

The Bash script ``scripts/perform_codec_comparison.sh`` can be used to compare the performance of GABAC to other tools. The scripts compresses and decompresses a test file using gzip, bzip2, xz (implementing the LZMA algorithm), rANS order 0 and rANS order 1 (see https://github.com/voges/rans.git), and gabacify when executing e.g. the following command from the ``scripts``directory:

    ./perform_codec_comparison ../resources/input_files/one_mebibyte_random

The compression and decompression times, the maximum RAM usage, and the compressed file sizes will be logged in the file ``../resources/input_files/one_mebibyte_random.codec_stats``.

**NOTE**: gabacify is designed to run on pieces of data which sizes lie below 1 GB. The entire input file will be read into memory and several buffers will be allocated. The estimated RAM usage for compressing a 1 GB file lies between 2 GB and 4 GB. To reduce RAM usage you can tell gabacify to split the input file into blocks of fixed size (using -b [Blocksize in bytes]). This can reduce compression performance but can be much more ressource friendly. Choose a blocksize divisible by 8 for optimal performance.

## Continuous integration

Commits to this repository are continuously tested on **Travis CI** (https://travis-ci.org/voges/gabac). Take a look at ``.travis.yml`` to see what is being done on Travis' (virtual) machines.

## Build system

We use **CMake** (https://cmake.org/) as build system and we provide a ``CMakeLists.txt`` to build GABAC.

## Version control system

### Branching

We use **Git** and we use the **Gitflow** workflow (https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow).

That means:

* The ``master`` branch contains only *release* commits.
* Every commit on the master branch is *tagged* according to **Semantic Versioning 2.0.0** (see below).
* Development generally takes place on the ``develop`` branch.
* Actual development takes place in *feature* branches, e.g., ``feature/my_fancy_feature``.
* Once a *feature* is completed, its branch can be merged back into the ``develop`` branch.

### Version numbers

We use the Semantic Versioning 2.0.0 (https://semver.org).

That means:

* The **release** version number format is: MAJOR.MINOR.PATCH
* Increment the
  * MAJOR version when you make incompatible API changes,
  * MINOR version when you add functionality in a backwards-compatible manner, and
  * PATCH version when you make backwards-compatible bug fixes.
* **Pre-release** versions are denoted by appending a hyphen and a series of dot separated identifiers immediately following the patch version.
  * Example 1: 1.0.0-alpha.1 ("alpha version 1 of the planned major release 1")
  * Example 2: 1.0.0-beta.1 ("beta version 1 of the planned major release 1")
  * Example 3: 1.0.0-rc.1 ("release candidate (rc) 1 of the planned major release 1")
  * Note: 1.0.0-alpha.1 < 1.0.0-beta.1 < 1.0.0-rc.1 by definition (see https://semver.org)

## Who do I talk to?

Jan Voges <[voges@tnt.uni-hannover.de](mailto:voges@tnt.uni-hannover.de)>

Tom Paridaens <[tom.paridaens@ugent.be](mailto:tom.paridaens@ugent.be)>

Mikel Hernaez <[mhernaez@illinois.edu](mhernaez@illinois.edu)>
