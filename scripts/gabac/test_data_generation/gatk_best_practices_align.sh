#!/usr/bin/env bash


# Perform alignment according to the GATK Best Practices
# See also: https://software.broadinstitute.org/gatk/best-practices/


self="$0"
self_name="${self##*/}"

datetime=$(date -u +"%Y-%m-%dT%H:%M:%SZ")

trace() { echo "[${self_name}] [$datetime] [trace] $*"; }
debug() { echo "[${self_name}] [$datetime] [debug] $*"; }
info() { echo "[${self_name}] [$datetime] [info] $*"; }
warning() { echo "[${self_name}] [$datetime] [warning] $*"; }
error() { echo "[${self_name}] [$datetime] [error] $*" 1>&2; }
fatal() { echo "[${self_name}] [$datetime] [fatal] $*" 1>&2; }

run() {
    local rc
    info "Running: $*"
    "$@"
    rc="$?"
    if [[ $rc != 0 ]]; then
        fatal "Command failed (RC=${rc}): $*"
        exit $rc
    fi
}


# -----------------------------------------------------------------------------
# Parameters
# -----------------------------------------------------------------------------

readonly num_threads=2

# For single-ended data just leave $input_fastq_2 empty.
readonly input_fastq_1="ERR174310_1.fastq"
readonly input_fastq_2="ERR174310_2.fastq"

# Output files will be:
#   ${output_prefix}.aln.sam
#   ${output_prefix}.aln.sort.bam
#   ${output_prefix}.aln.sort.dupmark.bam
#   ${output_prefix}.aln.sort.dupmark.rg.bam
#   ${output_prefix}.aln.sort.dupmark.rg.recal.bam
# Supplementary output files will be:
#   ${output_prefix}.dedup_metrics.txt
#   ${output_prefix}.aln.sort.dupmark.rg.bam.recal_data.table
readonly output_prefix="ERR174310"

# Tested with GATK bundle 2.8-b37.
readonly gatk_bundle_path="/path/to/GATK_bundle-2.8-b37"


# -----------------------------------------------------------------------------
# Required programs
# -----------------------------------------------------------------------------

readonly bwa="/project/omics/install/bwa-0.7.13/bwa"
readonly gatk="/project/omics/install/gatk-4.0.8.1/gatk"
readonly picard_jar="/project/omics/install/picard-tools-2.18.14/picard.jar"
readonly samtools="/project/omics/install/samtools-1.3/bin/samtools"


# -----------------------------------------------------------------------------
# Command line
# -----------------------------------------------------------------------------

if [[ $# != 0 ]]; then
    info "Usage: ${self_name}"
    info "(Hint: set parameters directly in the script)"
    exit 1
fi


# -----------------------------------------------------------------------------
# GATK bundle
# -----------------------------------------------------------------------------

readonly dbsnp_vcf="$gatk_bundle_path/dbsnp_138.b37.vcf"
readonly ref_fasta="$gatk_bundle_path/human_g1k_v37.fasta"

# Check if the reference is indexed.
if [[ ! -f "${ref_fasta}.fai" ]]; then
    error "Reference '${ref_fasta}' not indexed"
    info "(Hint: use 'samtools faidx ref.fasta')"
    exit 1
fi


# -----------------------------------------------------------------------------
# Do it
# -----------------------------------------------------------------------------

# Generate BWA index.
info "Generating BWA index"
run $bwa index -a bwtsw $ref_fasta

# Map reads to reference.
info "mapping reads to reference"
run $bwa mem \
    -t "$num_threads" \
    -M "$ref_fasta" \
    "$input_fastq_1" \
    "$input_fastq_2" \
    > "${output_prefix}.aln.sam"

# Sort SAM file and convert to BAM.
# printf "Sorting SAM file (while converting it to BAM)"
# run java -jar $picard_jar SortSam \
#     INPUT="${output_prefix}.aln.sam" \
#     OUTPUT="${output_prefix}.aln.sort.bam" \
#     SORT_ORDER=coordinate

# Sort SAM file and convert to BAM. This sorting (with Samtools) should be
# used if the above Picard call does not finish because of "too many open
# files". The "-m 2G" parameter to Samtools sort means that every *thread*
# will use up to 2 GB of RAM at the "expense" of using less temporary files.
info "Sorting SAM file (while converting it to BAM)"
run $samtools sort \
    -@ "$num_threads" \
    -m 2G \
    "${output_prefix}.aln.sam" \
    -o "${output_prefix}.aln.sort.bam" \
    -O bam

# Mark duplicates in the BAM file.
info "Marking duplicates"
run java -jar $picard_jar MarkDuplicates \
    INPUT="${output_prefix}.aln.sort.bam" \
    OUTPUT="${output_prefix}.aln.sort.dupmark.bam" \
    METRICS_FILE="${output_prefix}.dedup_metrics.txt" \
    ASSUME_SORTED=true

# Add read group.
info "Adding read group"
run java -jar "$picard_jar" AddOrReplaceReadGroups \
    INPUT="${output_prefix}.aln.sort.dupmark.bam" \
    OUTPUT="${output_prefix}.aln.sort.dupmark.rg.bam" \
    RGID=1 \
    RGLB=library \
    RGPL=illumina \
    RGPU=platform_unit \
    RGSM=sample_name

# Run the BaseRecalibrator.
info "Running GATK's BaseRecalibrator"
run $gatk BaseRecalibrator \
    --reference "$ref_fasta" \
    --input "${output_prefix}.aln.sort.dupmark.rg.bam" \
    --known-sites "$dbsnp_vcf" \
    --output "${output_prefix}.aln.sort.dupmark.rg.bam.recal_data.table"

# Create recalibrated BAM.
info "Creating recalibrated BAM"
run $gatk ApplyBQSR \
    --reference "$ref_fasta" \
    --input "${output_prefix}.aln.sort.dupmark.rg.bam" \
    --bqsr-recal-file "${output_prefix}.aln.sort.dupmark.rg.bam.recal_data.table" \
    --output "${output_prefix}.aln.sort.dupmark.rg.recal.bam" \
    --emit-original-quals


# -----------------------------------------------------------------------------
# Cleanup and exit
# -----------------------------------------------------------------------------

info "Success"
exit 0
