#!/usr/bin/env bash


# Perform alignment according to the GATK Best Practices
# See also: https://software.broadinstitute.org/gatk/best-practices/


# -----------------------------------------------------------------------------
# Generic
# -----------------------------------------------------------------------------

self="${0}"
self_name="${self##*/}"


# -----------------------------------------------------------------------------
# Parameters
# -----------------------------------------------------------------------------

readonly num_threads=2
readonly gatk_bundle_path="/path/to/GATK_bundle-2.8-b37"

# For single-ended data leave input_fastq_2 empty
readonly input_fastq_1="reads_1.fastq"
readonly input_fastq_2="reads_2.fastq"

# Output files will be:
#   ${output_prefix}.aln.sam
#   ${output_prefix}.aln.sort.bam
#   ${output_prefix}.aln.sort.dupmark.bam
#   ${output_prefix}.aln.sort.dupmark.rg.bam
#   ${output_prefix}.aln.sort.dupmark.rg.recal.bam
# Supplementary output files will be:
#   ${output_prefix}.dedup_metrics.txt
#   ${output_prefix}.aln.sort.dupmark.rg.bam.recal_data.table
readonly output_prefix="reads"


# -----------------------------------------------------------------------------
# Required programs
# -----------------------------------------------------------------------------

readonly bwa="/path/to/bwa-0.7.17/bwa"
readonly gatk="/path/to/gatk-4.1.2.0/gatk"
readonly picard_jar="/path/to/picard-2.20.2/picard.jar"
readonly samtools="/path/to/samtools-1.9/bin/samtools"


# -----------------------------------------------------------------------------
# Command line
# -----------------------------------------------------------------------------

if [[ "${#}" -ne 0 ]]; then
    echo "${self_name}: error: parameters must be specified directly in the script"
    exit 1
fi


# -----------------------------------------------------------------------------
# GATK bundle
# -----------------------------------------------------------------------------

readonly dbsnp_vcf="${gatk_bundle_path}/dbsnp_138.b37.vcf"
readonly ref_fasta="${gatk_bundle_path}/human_g1k_v37.fasta"

# Check if the reference is indexed
if [[ ! -f "${ref_fasta}.fai" ]]; then
    echo "${self_name}: error: reference '${ref_fasta}' not indexed"
    echo "${self_name}: hint: use 'samtools faidx ref.fasta'"
    exit 1
fi


# -----------------------------------------------------------------------------
# Do it
# -----------------------------------------------------------------------------

# Generate BWA index
echo "Generating BWA index"
"${bwa}" index -a bwtsw "${ref_fasta}" || exit 1

# Map reads to reference
echo "Mapping reads to reference"
"${bwa}" mem \
    -t "${num_threads}" \
    -M "${ref_fasta}" \
    "${input_fastq_1}" \
    "${input_fastq_2}" \
    > "${output_prefix}.aln.sam" || exit 1

# Sort SAM file and convert to BAM
# echo "Sorting SAM file (while converting it to BAM)"
# java -jar "${picard_jar}" SortSam \
#     INPUT="${output_prefix}.aln.sam" \
#     OUTPUT="${output_prefix}.aln.sort.bam" \
#     SORT_ORDER=coordinate || exit 1

# Sort SAM file and convert to BAM
#   This sorting (with Samtools) should be used if the above Picard call does
#   not finish because of "too many open files". The "-m 2G" parameter to
#   Samtools sort means that every *thread* will use up to 2 GB of RAM at the
#   "expense" of using less temporary files.
echo "Sorting SAM file (while converting it to BAM)"
"${samtools}" sort \
    -@ "${num_threads}" \
    -m 2G \
    "${output_prefix}.aln.sam" \
    -o "${output_prefix}.aln.sort.bam" \
    -O bam || exit 1

# Mark duplicates in the BAM file
echo "Marking duplicates"
java -jar "${picard_jar}" MarkDuplicates \
    INPUT="${output_prefix}.aln.sort.bam" \
    OUTPUT="${output_prefix}.aln.sort.dupmark.bam" \
    METRICS_FILE="${output_prefix}.dedup_metrics.txt" \
    ASSUME_SORTED=true || exit 1

# Add read group
echo "Adding read group"
java -jar "${picard_jar}" AddOrReplaceReadGroups \
    INPUT="${output_prefix}.aln.sort.dupmark.bam" \
    OUTPUT="${output_prefix}.aln.sort.dupmark.rg.bam" \
    RGID=1 \
    RGLB=library \
    RGPL=illumina \
    RGPU=platform_unit \
    RGSM=sample_name || exit 1

# Run the BaseRecalibrator
echo "Running GATK's BaseRecalibrator"
"${gatk}" BaseRecalibrator \
    --reference "${ref_fasta}" \
    --input "${output_prefix}.aln.sort.dupmark.rg.bam" \
    --known-sites "${dbsnp_vcf}" \
    --output "${output_prefix}.aln.sort.dupmark.rg.bam.recal_data.table" || exit 1

# Create recalibrated BAM
echo "Creating recalibrated BAM"
"${gatk}" ApplyBQSR \
    --reference "${ref_fasta}" \
    --input "${output_prefix}.aln.sort.dupmark.rg.bam" \
    --bqsr-recal-file "${output_prefix}.aln.sort.dupmark.rg.bam.recal_data.table" \
    --output "${output_prefix}.aln.sort.dupmark.rg.recal.bam" \
    --emit-original-quals || exit 1


# -----------------------------------------------------------------------------
# Cleanup and exit
# -----------------------------------------------------------------------------

echo "Success"
exit 0
