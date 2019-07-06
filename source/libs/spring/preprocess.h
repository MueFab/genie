#ifndef SPRING_PREPROCESS_H_
#define SPRING_PREPROCESS_H_

#include <string>
#include "util.h"
#include <fio/fastq-file-reader.h>

namespace spring {

void preprocess(fio::FastqFileReader *fastqFileReader1,
		fio::FastqFileReader *fastqFileReader2,
                const std::string &temp_dir, compression_params &cp);

}  // namespace spring

#endif  // SPRING_PREPROCESS_H_
