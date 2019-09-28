# Version control system

## Branching

We use Git and we use a workflow similiar to Gitflow (https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow).

That means:

* Some commits on the master branch are considered as being *release* commits. They are tagged according to Semantic Versioning 2.0.0 (see below).
* Development generally takes place on the ``develop`` branch.
* Actual development takes place in feature branches, e.g., ``feature/better-documentation``.
* Once a feature is completed, its branch can be merged back into the ``develop`` branch.

## Version numbers

We use the Semantic Versioning 2.0.0 (https://semver.org).

That means:

* The release version number format is: MAJOR.MINOR.PATCH
* We increment the
  * MAJOR version when making incompatible API changes,
  * MINOR version when adding functionality in a backwards-compatible manner, and
  * PATCH version when making backwards-compatible bug fixes.
* Pre-release versions are denoted by appending a hyphen and a series of dot separated identifiers immediately following the patch version.
  * Example 1: 1.0.0-alpha.1 ("alpha version 1 of the planned major release 1")
  * Example 2: 1.0.0-beta.1 ("beta version 1 of the planned major release 1")
  * Example 3: 1.0.0-rc.1 ("release candidate (rc) 1 of the planned major release 1")
  * Note: 1.0.0-alpha.1 < 1.0.0-beta.1 < 1.0.0-rc.1 by definition (see https://semver.org)
