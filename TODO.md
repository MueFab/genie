# Genie TODOs

## Build system

- [ ] [Jan] Enable builds on Travis CI (Jan)
- [ ] [Mingyu] Set up GNU Autotools (./configure; make; make install)
  - [ ] [Jan] Move GABAC to the genie repository (necessary to enable tidy building with GNU Autotools)
- [ ] [Jan] Clean up CMake (should be kept for development because it massively eases unit testing)

## Predefined SPRING stream configurations for GABAC
- [x] [Jan,Fabian] Merge branches
  - [ ] [Fabian] Fix SPRING decoder
- [ ] [Fabian] Incorporate information from Shubham’s documentation to skip additional GABAC checks

## SPRING multi-threading

- [ ] [Brian] Discuss with Shubham about the feasibility of block-wise compression with SPRING

## Paired-end file I/O

- [ ] [Brian] Use separate threads to do the reading
