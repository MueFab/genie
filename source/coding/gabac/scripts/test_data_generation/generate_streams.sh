#!/usr/bin/env bash

# Generate streams for GABAC simulations

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

if [[ $# != 0 ]]; then
    info "Usage: ${self_name}"
    exit 1
fi

readonly deez="/data/voges/bin/deez-v1.9-rawstreams"
readonly samtools="/data/voges/bin/samtools-1.9-rawstreams-itf8"

readonly mpeg_g_gidb_01_dir="/data/voges/data/mpeg-g-gidb-01"
readonly mpeg_g_gidb_01_11_bam="${mpeg_g_gidb_01_dir}/chromosomes/ERR17431.aln.sort.dupmark.rg.11.bam"
readonly mpeg_g_gidb_01_11_sam="${mpeg_g_gidb_01_dir}/chromosomes/ERR17431.aln.sort.dupmark.rg.11.sam"
readonly mpeg_g_gidb_01_20_bam="${mpeg_g_gidb_01_dir}/chromosomes/ERR17431.aln.sort.dupmark.rg.20.bam"
readonly mpeg_g_gidb_01_20_sam="${mpeg_g_gidb_01_dir}/chromosomes/ERR17431.aln.sort.dupmark.rg.20.sam"
readonly mpeg_g_gidb_01_ref="${mpeg_g_gidb_01_dir}/original/human_g1k_v37.fasta"
readonly mpeg_g_gidb_01_deez_dir="${mpeg_g_gidb_01_dir}/deez-v1.9-rawstreams"
readonly mpeg_g_gidb_01_cram_dir="${mpeg_g_gidb_01_dir}/cram-1.9-rawstreams-itf8"

readonly mpeg_g_gidb_02_dir="/data/voges/data/mpeg-g-gidb-02"
readonly mpeg_g_gidb_02_chr11_bam="${mpeg_g_gidb_02_dir}/chromosomes/NA12878_S1.chr11.bam"
readonly mpeg_g_gidb_02_chr11_sam="${mpeg_g_gidb_02_dir}/chromosomes/NA12878_S1.chr11.sam"
readonly mpeg_g_gidb_02_chr20_bam="${mpeg_g_gidb_02_dir}/chromosomes/NA12878_S1.chr20.bam"
readonly mpeg_g_gidb_02_chr20_sam="${mpeg_g_gidb_02_dir}/chromosomes/NA12878_S1.chr20.sam"
readonly mpeg_g_gidb_02_ref="${mpeg_g_gidb_02_dir}/original/hg19.fa"
readonly mpeg_g_gidb_02_deez_dir="${mpeg_g_gidb_02_dir}/deez-v1.9-rawstreams"
readonly mpeg_g_gidb_02_cram_dir="${mpeg_g_gidb_02_dir}/cram-1.9-rawstreams-itf8"

info "Running DeeZ on chromosomes 11 and 22 from MPEG-G GIDB item 01"
run mkdir "$mpeg_g_gidb_01_deez_dir"
run "$deez" -r "$mpeg_g_gidb_01_ref" "$mpeg_g_gidb_01_11_sam" -o "${mpeg_g_gidb_01_deez_dir}/${mpeg_g_gidb_01_11_sam##*/}.deez_rawstreams"
run "$deez" -r "$mpeg_g_gidb_01_ref" "$mpeg_g_gidb_01_20_sam" -o "${mpeg_g_gidb_01_deez_dir}/${mpeg_g_gidb_01_20_sam##*/}.deez_rawstreams"

info "Running DeeZ on chromosomes 11 and 22 from MPEG-G GIDB item 02"
run run mkdir "$mpeg_g_gidb_02_deez_dir"
run "$deez" -r "$mpeg_g_gidb_02_ref" "$mpeg_g_gidb_02_chr11_sam" -o "${mpeg_g_gidb_02_deez_dir}/${mpeg_g_gidb_02_chr11_sam##*/}.deez_rawstreams"
run "$deez" -r "$mpeg_g_gidb_02_ref" "$mpeg_g_gidb_02_chr20_sam" -o "${mpeg_g_gidb_02_deez_dir}/${mpeg_g_gidb_02_chr20_sam##*/}.deez_rawstreams"

info "Running CRAM on chromosomes 11 and 22 from MPEG-G GIDB item 01"
run mkdir "$mpeg_g_gidb_01_cram_dir"
run "$samtools" view -C -T "$mpeg_g_gidb_01_ref" "$mpeg_g_gidb_01_11_bam" -o "${mpeg_g_gidb_01_cram_dir}/${mpeg_g_gidb_01_11_bam##*/}.cram_rawstreams"
run "$samtools" view -C -T "$mpeg_g_gidb_01_ref" "$mpeg_g_gidb_01_11_bam" -o "${mpeg_g_gidb_01_cram_dir}/${mpeg_g_gidb_01_11_bam##*/}.cram_rawstreams"

info "Running CRAM on chromosomes 11 and 22 from MPEG-G GIDB item 02"
run mkdir "$mpeg_g_gidb_02_cram_dir"
run "$samtools" view -C -T "$mpeg_g_gidb_02_ref" "$mpeg_g_gidb_02_chr11_bam" -o "${mpeg_g_gidb_02_cram_dir}/${mpeg_g_gidb_02_chr11_bam##*/}.cram_rawstreams"
run "$samtools" view -C -T "$mpeg_g_gidb_02_ref" "$mpeg_g_gidb_02_chr20_bam" -o "${mpeg_g_gidb_02_cram_dir}/${mpeg_g_gidb_02_chr20_bam##*/}.cram_rawstreams"
