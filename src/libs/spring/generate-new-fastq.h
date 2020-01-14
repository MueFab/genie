#ifndef SPRING_GENERATE_NEW_FASTQ_H_
#define SPRING_GENERATE_NEW_FASTQ_H_

#include "util.h"

namespace genie {
namespace spring {

/*void generate_new_fastq_se(format::fastq::FastqFileReader *fastqFileReader1, const std::string &temp_dir,
                           const compression_params &cp, const std::string &outputFilePath);

void generate_new_fastq_pe(format::fastq::FastqFileReader *fastqFileReader1, format::fastq::FastqFileReader
*fastqFileReader2, const std::string &temp_dir, const compression_params &cp, const std::string &outputFilePath);*/

void generate_order_array(const std::string &file_order, uint32_t *order_array, const uint32_t &numreads);

}  // namespace spring
}  // namespace genie

#endif  // SPRING_GENERATE_NEW_FASTQ_H_
