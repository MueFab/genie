# Coding style

We use the **Google C++ Style Guide** available at https://google.github.io/styleguide/cppguide.html.

## Style checking

Conformance testing of a file with regard to the Google C++ Style Guide is done with the Python script **cpplint.py** (https://github.com/cpplint/cpplint).

Furthermore, we provide a cpplint configuration in the file ``source/CPPLINT.cfg``.

The testing of the entire code tree is automated via the Bash script ``extras/run_cpplint.sh``. To add new files to the testing they have to be added manually to the file ``run_cpplint.sh``.

## Style guide

The used coding style in a nutshell: look what's already there and try to use the same style.

### Indentation

Configure your editor to use **4 spaces** for indentation. Do not use tabs!

### Namespaces

The namespace ``gabac`` is used for the GABAC library. This prohibits problems with existing names and thereby allows shorter class/function/variable names.

Do not use using-directives):

```cpp
using namespace gabac;  // forbidden!
```

#### File names

All file names have lower-case letters only. Words are separated by an underscore. C++ files end on `.cpp`, header files end on `.h`.

#### Class names
Class names are in CamelCase, starting with a Capital letter.

#### Variable names

Variable names are in camelCase, starting with a lowercase letter. Class member variables have a prepended `m_`.

#### Function Names

Function names are in camelCase, starting with a lowercase letter

#### Acronyms

When using acronyms for CamelCase names only capitalize the first letter of an acronym:

```cpp
class DctTransformCoding {...};  // yes!
```

instead of

```cpp
class DCTTransformCoding {...};  // no!
```

### Line length

The maximal allowed line length is 80.
