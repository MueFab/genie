/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <genie/core/stats/perf-stats.h>
#include <iostream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace stats {

// ---------------------------------------------------------------------------------------------------------------------

// For debugging (but always enabled)
__attribute__((noinline)) void descNotHandled(int id, size_t size) {
    std::cerr << "Block desc #" << id << " of size " << size << " unaccounted for." << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

PerfStats::PerfStats(bool _decompression) {
    decompression = _decompression;
    num_recs = 0;
    total_t = std::chrono::steady_clock::duration::zero();
    preprocess_t = std::chrono::steady_clock::duration::zero();
    reorder_t = std::chrono::steady_clock::duration::zero();
    encode_t = std::chrono::steady_clock::duration::zero();
    generation_t = std::chrono::steady_clock::duration::zero();
    qual_score_t = std::chrono::steady_clock::duration::zero();
    combine_t = std::chrono::steady_clock::duration::zero();
    orig_total_sz = 0;
    cmprs_total_sz = 0;
}

PerfStats::~PerfStats(void) {
    // pure virtual ; can't be inlined
}

// ---------------------------------------------------------------------------------------------------------------------

FastqStats::FastqStats(bool _decompression) : PerfStats(_decompression) {
    orig_id_sz = 0;
    cmprs_id_sz = 0;
    orig_seq_sz = 0;
    cmprs_seq_sz = 0;
    orig_qual_sz = 0;
    cmprs_qual_sz = 0;
}

FastqStats::~FastqStats(void) {}

void FastqStats::recordMgbStreams(const std::vector<genie::format::mgb::Block> &blocks) {
    size_t id_sz = 0;
    size_t seq_sz = 0;
    size_t qual_sz = 0;
    size_t total_sz = 0;

    for (auto block : blocks) {
        size_t size = block.getWrittenSize();
        if (size == 0) {
            continue;
        }
        switch (block.getDescriptorID()) {
            case (int)GenDesc::RNAME:
                id_sz += size;
                break;
            case (int)GenDesc::UREADS:
                seq_sz += size;
                break;
            case (int)GenDesc::QV:
                qual_sz += size;
                break;
            default:
                descNotHandled(block.getDescriptorID(), size);
                break;
        }
        total_sz += size;
    }

    if (id_sz != 0) cmprs_id_sz += id_sz;
    if (seq_sz != 0) cmprs_seq_sz += seq_sz;
    if (qual_sz != 0) cmprs_qual_sz += qual_sz;
    if (total_sz != 0) cmprs_total_sz += total_sz;
}

void FastqStats::printStats(void) {
    if (!decompression) {
        std::cerr << "#FASTQ records: " << num_recs << std::endl;

        std::cerr.precision(3);
        std::cerr << "id:       compressed " << orig_id_sz << " bytes to " << cmprs_id_sz << " bytes ("
                  << ((double)orig_id_sz) / cmprs_id_sz << "x)" << std::endl;
        std::cerr << "seq:      compressed " << orig_seq_sz << " bytes to " << cmprs_seq_sz << " bytes ("
                  << ((double)orig_seq_sz) / cmprs_seq_sz << "x)" << std::endl;
        std::cerr << "qv:       compressed " << orig_qual_sz << " bytes to " << cmprs_qual_sz << " bytes ("
                  << ((double)orig_qual_sz) / cmprs_qual_sz << "x)" << std::endl;

        off_t orig_ovhd_sz = orig_total_sz - orig_id_sz - orig_seq_sz - orig_qual_sz;
        off_t cmprs_ovhd_sz = cmprs_total_sz - cmprs_id_sz - cmprs_seq_sz - cmprs_qual_sz;
        std::cerr << "Overhead: " << orig_ovhd_sz << " bytes (" << 100.0 * orig_ovhd_sz / orig_total_sz << "%) / "
                  << cmprs_ovhd_sz << " bytes (" << 100.0 * cmprs_ovhd_sz / cmprs_total_sz << "%)" << std::endl;
        std::cerr << "Total:    compressed " << orig_total_sz << " bytes to " << cmprs_total_sz << " bytes ("
                  << (((double)orig_total_sz) / cmprs_total_sz) << "x)" << std::endl;

        std::cerr << "Preprocessing:  " << std::chrono::duration_cast<std::chrono::milliseconds>(preprocess_t).count()
                  << " msec" << std::endl;
        std::cerr << "Reordering:     " << std::chrono::duration_cast<std::chrono::milliseconds>(reorder_t).count()
                  << " msec" << std::endl;
        std::cerr << "Encoding:       " << std::chrono::duration_cast<std::chrono::milliseconds>(encode_t).count()
                  << " msec" << std::endl;
        std::cerr << "Generation:     " << std::chrono::duration_cast<std::chrono::milliseconds>(generation_t).count()
                  << " msec" << std::endl;
        std::cerr << "Quality scores: " << std::chrono::duration_cast<std::chrono::milliseconds>(qual_score_t).count()
                  << " msec" << std::endl;
        std::cerr << "Combine AUs:    " << std::chrono::duration_cast<std::chrono::milliseconds>(combine_t).count()
                  << " msec" << std::endl;

        std::cerr << "Total time:     " << std::chrono::duration_cast<std::chrono::milliseconds>(total_t).count()
                  << " msec" << std::endl;
    } else {
        std::cerr << "<Decompression stats not written yet>" << std::endl;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

SamStats::SamStats(bool _decompression) : PerfStats(_decompression) {
    orig_qname_sz = 0;
    cmprs_qname_sz = 0;
    orig_flag_sz = 0;
    cmprs_flag_sz = 0;
    orig_rname_sz = 0;
    cmprs_rname_sz = 0;
    orig_pos_sz = 0;
    cmprs_pos_sz = 0;
    orig_mapq_sz = 0;
    cmprs_mapq_sz = 0;
    orig_cigar_sz = 0;
    cmprs_cigar_sz = 0;
    orig_rnext_sz = 0;
    cmprs_rnext_sz = 0;
    orig_pnext_sz = 0;
    cmprs_pnext_sz = 0;
    orig_tlen_sz = 0;
    cmprs_tlen_sz = 0;
    orig_seq_sz = 0;
    cmprs_seq_sz = 0;
    orig_qual_sz = 0;
    cmprs_qual_sz = 0;
}

SamStats::~SamStats(void) {}

void SamStats::recordMgbStreams(const std::vector<genie::format::mgb::Block> &blocks) {
    size_t qname_sz = 0;
    size_t flag_sz = 0;
    size_t rname_sz = 0;
    size_t pos_sz = 0;
    size_t mapq_sz = 0;
    size_t cigar_sz = 0;
    size_t rnext_sz = 0;
    size_t pnext_sz = 0;
    size_t tlen_sz = 0;
    size_t seq_sz = 0;
    size_t qual_sz = 0;
    size_t rlen_sz = 0;
    size_t total_sz = 0;

    for (auto block : blocks) {
        size_t size = block.getWrittenSize();
        if (size == 0) {
            continue;
        }
        switch (block.getDescriptorID()) {
            // case (int)GenDesc::qname:  qname_sz += size; break;
            case (int)GenDesc::FLAGS:
                flag_sz += size;
                break;
            case (int)GenDesc::RNAME:
                rname_sz += size;
                break;
            case (int)GenDesc::POS:
                pos_sz += size;
                break;
            case (int)GenDesc::MSCORE:
                mapq_sz += size;
                break;
            case (int)GenDesc::CLIPS:
                cigar_sz += size;
                break;
            // case (int)GenDesc::rnext:  rnext_sz += size; break;
            // case (int)GenDesc::pnext:  pnext_sz += size; break;
            // case (int)GenDesc::tlen:   tlen_sz += size; break;
            case (int)GenDesc::UREADS:
                seq_sz += size;
                break;
            case (int)GenDesc::QV:
                qual_sz += size;
                break;
            case (int)GenDesc::RLEN:
                rlen_sz += size;
                break;
            default:
                descNotHandled(block.getDescriptorID(), size);
                break;
        }
        total_sz += size;
    }

    if (qname_sz != 0) cmprs_qname_sz += qname_sz;
    if (flag_sz != 0) cmprs_flag_sz += flag_sz;
    if (rname_sz != 0) cmprs_rname_sz += rname_sz;
    if (pos_sz != 0) cmprs_pos_sz += pos_sz;
    if (mapq_sz != 0) cmprs_mapq_sz += mapq_sz;
    if (cigar_sz != 0) cmprs_cigar_sz += cigar_sz;
    if (rnext_sz != 0) cmprs_rnext_sz += rnext_sz;
    if (pnext_sz != 0) cmprs_pnext_sz += pnext_sz;
    if (tlen_sz != 0) cmprs_tlen_sz += tlen_sz;
    if (seq_sz != 0) cmprs_seq_sz += seq_sz;
    if (qual_sz != 0) cmprs_qual_sz += qual_sz;
    if (rlen_sz != 0) {
        // split the rlen size contribs between the seqs and qual scores
        cmprs_seq_sz += rlen_sz / 2;
        cmprs_qual_sz += rlen_sz / 2;
    }
    if (total_sz != 0) cmprs_total_sz += total_sz;
}

void SamStats::printStats(void) {
    if (!decompression) {
        std::cerr << "#SAM records: " << num_recs << std::endl;

        std::cerr.precision(3);

        std::cerr << "qname:    compressed " << orig_qname_sz << " bytes to " << cmprs_qname_sz << " bytes ("
                  << ((double)orig_qname_sz) / cmprs_qname_sz << "x)" << std::endl;
        std::cerr << "flag:     compressed " << orig_flag_sz << " bytes to " << cmprs_flag_sz << " bytes ("
                  << ((double)orig_flag_sz) / cmprs_flag_sz << "x)" << std::endl;
        std::cerr << "rname:    compressed " << orig_rname_sz << " bytes to " << cmprs_rname_sz << " bytes ("
                  << ((double)orig_rname_sz) / cmprs_rname_sz << "x)" << std::endl;
        std::cerr << "pos:      compressed " << orig_pos_sz << " bytes to " << cmprs_pos_sz << " bytes ("
                  << ((double)orig_pos_sz) / cmprs_pos_sz << "x)" << std::endl;
        std::cerr << "mapq:     compressed " << orig_mapq_sz << " bytes to " << cmprs_mapq_sz << " bytes ("
                  << ((double)orig_mapq_sz) / cmprs_mapq_sz << "x)" << std::endl;
        std::cerr << "cigar:    compressed " << orig_cigar_sz << " bytes to " << cmprs_cigar_sz << " bytes ("
                  << ((double)orig_cigar_sz) / cmprs_cigar_sz << "x)" << std::endl;
        std::cerr << "rnext:    compressed " << orig_rnext_sz << " bytes to " << cmprs_rnext_sz << " bytes ("
                  << ((double)orig_rnext_sz) / cmprs_rnext_sz << "x)" << std::endl;
        std::cerr << "pnext:    compressed " << orig_pnext_sz << " bytes to " << cmprs_pnext_sz << " bytes ("
                  << ((double)orig_pnext_sz) / cmprs_pnext_sz << "x)" << std::endl;
        std::cerr << "tlen:     compressed " << orig_tlen_sz << " bytes to " << cmprs_tlen_sz << " bytes ("
                  << ((double)orig_tlen_sz) / cmprs_tlen_sz << "x)" << std::endl;
        std::cerr << "seq:      compressed " << orig_seq_sz << " bytes to " << cmprs_seq_sz << " bytes ("
                  << ((double)orig_seq_sz) / cmprs_seq_sz << "x)" << std::endl;
        std::cerr << "qv:       compressed " << orig_qual_sz << " bytes to " << cmprs_qual_sz << " bytes ("
                  << ((double)orig_qual_sz) / cmprs_qual_sz << "x)" << std::endl;

        off_t orig_ovhd_sz = orig_total_sz - orig_qname_sz - orig_flag_sz - orig_rname_sz - orig_pos_sz - orig_mapq_sz -
                             orig_cigar_sz - orig_rnext_sz - orig_pnext_sz - orig_tlen_sz - orig_seq_sz - orig_qual_sz;
        off_t cmprs_ovhd_sz = cmprs_total_sz - cmprs_qname_sz - cmprs_flag_sz - cmprs_rname_sz - cmprs_pos_sz -
                              cmprs_mapq_sz - cmprs_cigar_sz - cmprs_rnext_sz - cmprs_pnext_sz - cmprs_tlen_sz -
                              cmprs_seq_sz - cmprs_qual_sz;
        std::cerr << "Overhead: " << orig_ovhd_sz << " bytes (" << 100.0 * orig_ovhd_sz / orig_total_sz << "%) / "
                  << cmprs_ovhd_sz << " bytes (" << 100.0 * cmprs_ovhd_sz / cmprs_total_sz << "%)" << std::endl;
        std::cerr << "Total:    compressed " << orig_total_sz << " bytes to " << cmprs_total_sz << " bytes ("
                  << (((double)orig_total_sz) / cmprs_total_sz) << "x)" << std::endl;

        std::cerr << "Preprocessing:  " << std::chrono::duration_cast<std::chrono::milliseconds>(preprocess_t).count()
                  << " msec" << std::endl;
        std::cerr << "Reordering:     " << std::chrono::duration_cast<std::chrono::milliseconds>(reorder_t).count()
                  << " msec" << std::endl;
        std::cerr << "Encoding:       " << std::chrono::duration_cast<std::chrono::milliseconds>(encode_t).count()
                  << " msec" << std::endl;
        std::cerr << "Generation:     " << std::chrono::duration_cast<std::chrono::milliseconds>(generation_t).count()
                  << " msec" << std::endl;
        std::cerr << "Quality scores: " << std::chrono::duration_cast<std::chrono::milliseconds>(qual_score_t).count()
                  << " msec" << std::endl;
        std::cerr << "Combine AUs:    " << std::chrono::duration_cast<std::chrono::milliseconds>(combine_t).count()
                  << " msec" << std::endl;

        std::cerr << "Total time:     " << std::chrono::duration_cast<std::chrono::milliseconds>(total_t).count()
                  << " msec" << std::endl;
    } else {
        std::cerr << "<Decompression stats not written yet>" << std::endl;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace stats
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
