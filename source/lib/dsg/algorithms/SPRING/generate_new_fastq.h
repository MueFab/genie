#ifndef SPRING_GENERATE_NEW_FASTQ_H_
#define SPRING_GENERATE_NEW_FASTQ_H_

#include "algorithms/SPRING/util.h"
#include "input/fastq/FastqFileReader.h"

namespace spring {

void generate_new_fastq_se(dsg::input::fastq::FastqFileReader *fastqFileReader1,
  const std::string &temp_dir, const compression_params &cp);

} // namespace spring

#endif // SPRING_GENERATE_NEW_FASTQ_H_
