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

## Who do I talk to?

Jan Voges <[voges@tnt.uni-hannover.de](mailto:voges@tnt.uni-hannover.de)>

Tom Paridaens <[tom.paridaens@ugent.be](mailto:tom.paridaens@ugent.be)>

Mikel Hernaez <[mhernaez@illinois.edu](mhernaez@illinois.edu)>
