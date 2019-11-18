
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <string>

#include "perf-stats.h"

namespace util {

PerfStats::~PerfStats(void) {
    // pure virtual ; can't be inlined
}

void FastqStats::printCompressionStats(void) {
    std::cerr << "#FASTQ records: " << num_recs << std::endl;

    std::cerr.precision(3);
    std::cerr << "Ids:            compressed " << orig_id_sz << " bytes to " << cmprs_id_sz << " bytes ("
              << ((double)orig_id_sz) / cmprs_id_sz << "x)" << std::endl;
    std::cerr << "Sequences:      compressed " << orig_seq_sz << " bytes to " << cmprs_seq_sz << " bytes ("
              << ((double)orig_seq_sz) / cmprs_seq_sz << "x)" << std::endl;
    std::cerr << "Qual scores:    compressed " << orig_qual_sz << " bytes to " << cmprs_qual_sz << " bytes ("
              << ((double)orig_qual_sz) / cmprs_qual_sz << "x)" << std::endl;
    std::cerr << "Overhead:       " << orig_total_sz - orig_id_sz - orig_seq_sz - orig_qual_sz << " bytes / "
              << cmprs_total_sz - cmprs_id_sz - cmprs_seq_sz - cmprs_qual_sz << " bytes" << std::endl;
    std::cerr << "Total:          compressed " << orig_total_sz << " bytes to " << cmprs_total_sz << " bytes ("
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
    std::cerr << "Total time:     " << std::chrono::duration_cast<std::chrono::milliseconds>(total_t).count() << " msec"
              << std::endl;
}

void FastqStats::printDecompressionStats(void) { std::cerr << "<Decompression stats not written yet>" << std::endl; }

}  // namespace util
