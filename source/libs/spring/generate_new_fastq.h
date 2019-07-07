#ifndef SPRING_GENERATE_NEW_FASTQ_H_
#define SPRING_GENERATE_NEW_FASTQ_H_

#include "spring/util.h"
#include "fileio/fastq_file_reader.h"

namespace spring {

    void generate_new_fastq_se(dsg::input::fastq::FastqFileReader *fastqFileReader1,
                               const std::string &temp_dir, const compression_params &cp);

    void generate_new_fastq_pe(dsg::input::fastq::FastqFileReader *fastqFileReader1,
                               dsg::input::fastq::FastqFileReader *fastqFileReader2,
                               const std::string &temp_dir, const compression_params &cp);

    void generate_order_array(const std::string &file_order, uint32_t *order_array,
                              const uint32_t &numreads);

} // namespace spring

#endif // SPRING_GENERATE_NEW_FASTQ_H_
