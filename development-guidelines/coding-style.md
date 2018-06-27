# Coding style

We use the **Google C++ Style Guide** available at https://google.github.io/styleguide/cppguide.html.

Conformance (and static code analysis) testing of a source or header file with regard to the Google C++ Style Guide is done with the Python script **cpplint.py** (https://github.com/cpplint/cpplint). A copy of this script can be found in the folder ``cpplint``.

Furthermore, we provide a cpplint configuration in the file ``source/CPPLINT.cfg``.

The testing of the entire source code is automated via the Bash script ``cpplint/run-cpplint.sh``, which must be executed from the ``cpplint`` folder.

