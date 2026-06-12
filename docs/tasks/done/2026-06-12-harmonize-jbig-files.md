# [TODO]: Harmonize JBIG C Wrapper Files

## Goal
Consolidate `jbig_c_wrapper.c` and `jbig_interface.c` into a single file `jbig_interface.c` to improve code organization and build simplification.

## Context
Currently, the JBIG C wrapper functions are split between two implementation files (`jbig_c_wrapper.c` and `jbig_interface.c`). Consolidating these into `jbig_interface.c` simplifies compilation, avoids duplicate includes, and aligns the source structure with `jbig_interface.h`.

## Technical Strategy
1. **Merge Implementations**: Concatenate all helper methods from `jbig_interface.c` and wrapper functions from `jbig_c_wrapper.c` into `jbig_interface.c`.
2. **Update CMakeLists**: Update `src/genie/entropy/jbig/CMakeLists.txt` to compile `jbig_interface.c` only and omit `jbig_c_wrapper.c`.
3. **Remove redundant file**: Delete `jbig_c_wrapper.c`.
4. **Verify**: Ensure that the `genie-jbig` library builds and all unit tests pass.

## Task Breakdown
| Step | Task | Action | Status | Dependencies |
| :--- | :--- | :--- | :--- | :--- |
| **A** | Merge implementation into `jbig_interface.c` | `[x]` Merge both C source files | Completed | - |
| **B** | Update CMake list | `[x]` Update `jbig/CMakeLists.txt` source files | Completed | A |
| **C** | Remove `jbig_c_wrapper.c` | `[x]` Delete `jbig_c_wrapper.c` from filesystem | Completed | B |
| **D** | Verify Build & Tests | `[x]` Compile and pass all unit tests | Completed | C |

## Parallel Development Plan
- None (linear workflow).

## Dependencies
- Step A -> Step B -> Step C -> Step D

## Definition of Done
- [ ] `jbig_interface.c` contains all helper callbacks and compress/decompress routines.
- [ ] `jbig_c_wrapper.c` is removed from the directory and CMake configuration.
- [ ] All unit and integration tests compile and pass.

## Debugging Experience
| Problem | Failed Solution | Successful Solution |
| :--- | :--- | :--- |

## Technical Debt / Future Work
- None.
