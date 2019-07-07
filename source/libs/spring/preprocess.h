#ifndef SPRING_PREPROCESS_H_
#define SPRING_PREPROCESS_H_

#include <string>
#include "spring/util.h"
#include "fileio/fastq_file_reader.h"

namespace spring {

    void preprocess(dsg::input::fastq::FastqFileReader *fastqFileReader1,
                    dsg::input::fastq::FastqFileReader *fastqFileReader2,
                    const std::string &temp_dir, compression_params &cp);

}  // namespace spring

#endif  // SPRING_PREPROCESS_H_
