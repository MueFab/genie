#!/bin/bash

file=$1

python="/usr/bin/python"

cpplint="cpplint.py"

cpplint_opts="--extensions=c,cc,cpp,h,hpp"
cpplint_opts+="--headers=h,hpp"
#cpplint_opts+="--linelength=120"
cpplint_opts+="--filter=-build/header_guard"

echo "Running cpplint on: $file";

$python $cpplint $cpplint_opts $file;
