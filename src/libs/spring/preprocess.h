#ifndef SPRING_PREPROCESS_H_
#define SPRING_PREPROCESS_H_

#include <format/fastq/fastq-file-reader.h>
#include <string>
#include "util.h"

namespace spring {

void preprocess(format::fastq::FastqFileReader *fastqFileReader1, format::fastq::FastqFileReader *fastqFileReader2,
                const std::string &temp_dir, compression_params &cp);

}  // namespace spring

#endif  // SPRING_PREPROCESS_H_
