#!/bin/bash

result="$(pwd)/test_results.txt"
git_root_dir="$(git rev-parse --show-toplevel)"
data_path="${git_root_dir}/data/test_data"

if [[ ! -x $MPEGG_REF_DECODER ]]; then
	echo "MPEG-G reference decoder could not be found. Please set environment variable MPEGG_REF_DECODER."
	exit 1
fi

touch "$result"
> "$result"

function execute_fastq_roundtrip() {

	echo "Executing test case ${1}:" >> "$result"

	# get file names and set path for roundtrip
	cd "${1}"
	files=($(ls *.fastq 2>/dev/null))
	path="ci/fastq_tools/fastq_roundtrip.sh"
	if [[ "${files[0]}" == "" ]]; then
		files=($(ls *.gz))
		path="ci/fastq_tools/fastq_gzip_roundtrip.sh"
	fi
	file_1="${1}/${files[0]}"
	file_2=""
	if [[ -f "${1}/${files[1]}" ]]; then
		file_2="${1}/${files[1]}"
	fi

	cd "$git_root_dir"
	touch "log.txt"

	# perform roundtrip
	eval "$path" "$file_1" "$file_2" &>> "log.txt"
		if [ $? -ne 0 ]; then
		  echo "Roundtrip failed. Here are the last 20 lines of the output:" >> "$result"
		  cat log.txt >> "$result"
			echo "$(basename ${1})	"
		else
    	echo "Roundtrip ended successfully." >> "$result"
			echo "$(basename ${1})    pass"
		fi

	rm "log.txt"

	{
	echo "___________________________________________________________________________________________"
	echo ""
	} >> "$result"

}

function execute_sam_roundtrip() {

	echo "Executing test case ${1}:" >> "$result"

	# get file names
	cd "${1}"
	files=($(ls *.sam *.bam *.cram *.fa 2>/dev/null))
	file_1="${1}/${files[0]}"
	fasta_file=""
	if [[ ${files[1]} != "" ]]; then
	  fasta_file="${1}/${files[1]}"
	fi

  path="ci/sam_tools/sam_roundtrip.sh"

	cd "$git_root_dir"
	touch "log.txt"

	# perform roundtrip
	eval "$path" "$file_1" "$fasta_file" &>> "log.txt"
		if [ $? -ne 0 ]; then
    			echo "Roundtrip failed. Here are the last 20 lines of the output:" >> "$result"
    			cat log.txt >> "$result"
    			echo "$(basename ${1})	"
		else
    			echo "Roundtrip ended successfully." >> "$result"
			echo "$(basename ${1})    pass"
		fi

	rm "log.txt"

	{
	echo "___________________________________________________________________________________________"
	echo ""
	} >> "$result"
}

echo "----- Fastq Tests Started -----" >> "$result"

for file in "${data_path}/expected_to_pass/fastq"/*; do
  execute_fastq_roundtrip $file
done

echo "----- Sam Tests Started -----" >> "$result"

for file in "${data_path}/expected_to_pass/sam"/*; do
  execute_sam_roundtrip $file
  rm -f $file/*.sam_sorted 2>/dev/null
done

rm -rf ${git_root_dir}/tmp.* 2>/dev/null
rm -f ${git_root_dir}/output* 2>/dev/null

echo "----- All tests completed -----" >> "$result"
