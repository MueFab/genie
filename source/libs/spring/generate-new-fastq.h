#ifndef SPRING_GENERATE_NEW_FASTQ_H_
#define SPRING_GENERATE_NEW_FASTQ_H_

#include <utils/fastq-file-reader.h>
#include "util.h"

namespace spring {

void generate_new_fastq_se(utils::FastqFileReader *fastqFileReader1, const std::string &temp_dir,
                           const compression_params &cp);

void generate_new_fastq_pe(utils::FastqFileReader *fastqFileReader1, utils::FastqFileReader *fastqFileReader2,
                           const std::string &temp_dir, const compression_params &cp);

void generate_order_array(const std::string &file_order, uint32_t *order_array, const uint32_t &numreads);

}  // namespace spring

#endif  // SPRING_GENERATE_NEW_FASTQ_H_
