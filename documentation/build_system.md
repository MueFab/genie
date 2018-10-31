# Build system

We use CMake (https://cmake.org/) as build system and we provide a ``CMakeLists.txt`` to build GABAC, which includes the following parts:

* **GABAC library**
  * transformations
  * binarizations
  * context selection
  * binary arithmetic coding
  * bitstream input/output
* **gabacify application**
  * command line interface application
  * demonstrates the usage of the GABAC library

Notes:

* New files must be added manually to the ``CMakeLists.txt``.
* GABAC is built as *static* library by default. To build GABAC as *shared* library supply ``GABAC_BUILD_SHARED_LIB=ON`` to CMake.

The CMake build was tested on the following systems:

| Operating system                                          | Compiler(s)                                                              |
| --------------------------------------------------------- | ------------------------------------------------------------------------ |
| openSUSE Leap 15.0                                        | gcc version 7.3.1 20180323 \[gcc-7-branch revision 258812\] (SUSE Linux) |
| Ubuntu 18.04.1 LTS                                        | gcc version 7.3.0 (Ubuntu 7.3.0-16ubuntu3)                               |
| macOS 10.13.6                                             | Apple LLVM version 9.1.0 (clang-902.0.39.2)                              |
| Microsoft Windows 10 Pro (Version 10.0.17134 Build 17134) | Microsoft Visual C++ 2017 - 00369-60000-00001-AA799                      |

## Linux

Build the GABAC library from the command line with the following commands.

    cd gabac
    mkdir build
    cd build
    cmake ..
    make

This generates ``build/libgabac.[a|so]`` (static or shared GABAC library).

Build the gabacify application from the command line with the following command.

    make gabacify

This generates ``build/gabacify`` (gabacify application).

Build the Doxygen documentation for the GABAC library with the following command.

    make doc

This generates ``build/doxygen_documentation`` (Doxygen HTML documentation)

## macOS

When using the command line, follow the steps outlined for Linux.

Alternatively, fire up the CMake GUI, locate the source code (e.g. ``/Users/janvoges/Repositories/gabac``) and specify where to build the binaries (e.g. ``/Users/janvoges/Repositories/gabac/build``). Click on 'Configure' and then on 'Generate'. This leaves the Xcode project file ``gabac.xcodeproj`` in the ``build`` directory. Open it by double-clicking on ``gabac.xcodeproj`` or simply hit "Open Project" in the CMake GUI.

## Windows

Fire up the CMake GUI, locate the source code (e.g. ``C:\Users\janvoges\Repositories\gabac``) and specify where to build the binaries (e.g. ``C:\Users\janvoges\Repositories\gabac\build``). Click on 'Configure' and then on 'Generate'. This leaves the Visual Studio Solution ``gabac.sln`` in the ``build`` directory. Open it by double-clicking on ``gabac.sln`` or simply hit "Open Project" in the CMake GUI.
