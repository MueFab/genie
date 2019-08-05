#ifndef SPRING_COMPRESS_UREADS_H_
#define SPRING_COMPRESS_UREADS_H_

#include <string>
#include "util.h"
#include <utils/fastq-file-reader.h>
#include <genie/stream-saver.h>

namespace spring {

    std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>>
    compress_ureads(utils::FastqFileReader *fastqFileReader1,
                    utils::FastqFileReader *fastqFileReader2,
                    const std::string &temp_dir, compression_params &cp,
                    dsg::StreamSaver& st);

}  // namespace spring

#endif  // SPRING_COMPRESS_UREADS_H_
