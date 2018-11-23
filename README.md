# genie

Open source MPEG-G codec

[![Build Status](https://travis-ci.com/voges/genie.svg?token=GduwvyBbRmwxKZ5dTnUU&branch=master)](https://travis-ci.com/voges/genie)

---

## Quick start on Linux

Build the GABAC library and the gabacify application:

    mkdir build
    cd build
    cmake ..
    make

Perform a test roundtrip from the ``build`` directory:

    ./genie -t encode -i reads.fastq -o reads.mgf
    ./genie -t decode -i reads.mgf -o reads.mgr

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
