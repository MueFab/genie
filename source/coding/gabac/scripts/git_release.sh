#!/usr/bin/env bash

# Create a new release on the master branch, "moving" the latest commit on
# the develop branch to the master branch. The new commit on the master branch
# is tagged with 'version_string'. Both the new commit on the master branch
# and the new tag are then pushed to 'origin'.

if [ "$#" -ne 1 ]; then
    printf "Usage: %s version_string\\n" "$0"; exit 1
fi

printf "== Checking out 'master'\\n"
git checkout master || exit 1
printf "== Merging latest commit from 'develop' into 'master'\\n"
git merge develop --squash --strategy=recursive --strategy-option=theirs || exit 1
printf "== Creating new commit on 'master' with commit message '%s'\\n" "$1"
git commit --message "$1" || exit 1
printf "== Tagging the just created commit on 'master' with '%s'\\n" "$1"
git tag "$1" || exit 1
printf "== If everything looks correct please manually push everything: "
printf "git push origin master && git push origin %s\\n" "$1"
