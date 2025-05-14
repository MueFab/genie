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

	# get file names
	
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

	cd ""
	echo "Executing test case ${1}:" >> "$result"

	# get file names
	cd "${1}"
	files=($(ls *.sam *.bam *.cram *.fa 2>/dev/null))
	file_1="${1}/${files[0]}"
	fasta_file=""
	if [[ ${files[1]} != "" ]]; then
	  fasta_file="${1}/${files[1]}"
	fi

	cd "$git_root_dir"
	touch "log.txt"

	path="ci/sam_tools/sam_roundtrip.sh"

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
	cd "${data_path}"

	{
	echo "___________________________________________________________________________________________"
	echo ""
	} >> "$result"
}

for i in $(seq -w 1 55); do
  if [ ! -d "${data_path}/${i}" ]; then
    continue
  fi
  if [ $i -le 12 ]; then
    execute_fastq_roundtrip "${data_path}/${i}"
  else
    execute_sam_roundtrip "${data_path}/${i}"
  fi
done
echo "----- All tests completed -----" >> "$result"
