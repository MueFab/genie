#ifndef SPRING_SPRING_H_
#define SPRING_SPRING_H_

#include <map>
#include <string>
#include <vector>

#include <genie/generation.h>
#include <genie/stream-saver.h>
#include <util/fastq-file-reader.h>
#include <util/perf-stats.h>

#include "return-structures.h"
#include "util.h"

namespace spring {

void generate_streams_SPRING(util::FastqFileReader *fastqFileReader1, util::FastqFileReader *fastqFileReader2,
                             int num_thr, bool paired_end, const std::string &working_dir, bool analyze,
                             const std::string &outputFilePath, bool ureads_flag, bool preserve_quality,
                             bool preserve_id);

void call_reorder(const std::string &temp_dir, compression_params &cp);

void call_encoder(const std::string &temp_dir, compression_params &cp);

std::string random_string(size_t length);

}  // namespace spring

#endif  // SPRING_SPRING_H_
