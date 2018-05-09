# Version control system

We use **Git** and we use the **Gitflow** workflow (https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow).

That means:

* The ``master`` branch contains only *release* commits. Every commit on the master branch is *tagged* (i.e. '1.4.2').
* Development takes place on the ``develop``branch.
* Actual development takes place in *feature* branches, e.g. ``feature/encoder``.
* Once a *feature* is completed, its branch can be merged back into the ``develop`` branch.
