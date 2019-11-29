#ifndef SPRING_COMPRESS_UREADS_H_
#define SPRING_COMPRESS_UREADS_H_

#include <genie/stream-saver.h>
#include <format/fastq/fastq-file-reader.h>
#include <util/perf-stats.h>
#include <string>
#include "util.h"

namespace spring {

void compress_ureads(format::fastq::FastqFileReader *fastqFileReader1, format::fastq::FastqFileReader *fastqFileReader2,
                     const std::string &temp_dir, compression_params &cp, const std::string &outputFilePath);

}  // namespace spring

#endif  // SPRING_COMPRESS_UREADS_H_
