#ifndef SPRING_COMPRESS_UREADS_H_
#define SPRING_COMPRESS_UREADS_H_

#include <genie/stream-saver.h>
#include <util/fastq-file-reader.h>
#include <util/perf-stats.h>
#include <string>
#include "util.h"

namespace spring {

void compress_ureads(util::FastqFileReader *fastqFileReader1, util::FastqFileReader *fastqFileReader2,
                     const std::string &temp_dir, compression_params &cp, const std::string &outputFilePath);

}  // namespace spring

#endif  // SPRING_COMPRESS_UREADS_H_
