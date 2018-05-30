# Build system

We use CMake (https://cmake.org/) as build system.

genie comprises two applications:

* dsg (descriptor-stream-generator)
* encoder

We provide a ``CMakeLists.txt`` file to build these applications with CMake (https://cmake.org/).

Build the applications from the command line with the following commands; alternatively use the CMake GUI.

    mkdir build
    cd build
    cmake ..
    make

This generates executables named ``dsg`` and ``encoder`` in the ``build`` folder.
