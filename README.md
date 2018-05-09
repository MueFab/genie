# genie

**GEN**omic **I**nformation **E**ncoding

---

This is the official repository for the development of genie.

## Quick start

genie comprises two applications:

* descriptor-stream-generator
* encoder

We provide two ``CMakeLists.txt`` files to build these applications with CMake (https://cmake.org/).

Build the descriptor-stream-generator from the command line with the following commands; alternatively use the CMake GUI.

    cd descriptor-stream-generator
    mkdir build
    cd build
    cmake ..
    make dsg

This generates an executable named ``dsg`` in the ``descriptor-stream-generator/build`` folder.

Build the encoder from the command line with the following commands; alternatively use the CMake GUI.

    cd encoder
    mkdir build
    cd build
    cmake ..
    make encoder

This generates an executable named ``encoder`` in the ``encoder/build`` folder.
