#ifndef SPRING_SPRING_H_
#define SPRING_SPRING_H_

#include <string>
#include "spring/util.h"
#include <utils/fastq-file-reader.h>
#include <vector>
#include <map>
#include <genie/generation.h>
#include "ReturnStructures.h"
#include "genie/StreamSaver.h"

namespace spring {

    generated_aus generate_streams_SPRING(
            utils::FastqFileReader *fastqFileReader1,
            utils::FastqFileReader *fastqFileReader2, int num_thr,
            bool paired_end, const std::string &working_dir, bool analyze, dsg::StreamSaver &st);

    void call_reorder(const std::string &temp_dir, compression_params &cp);

    void call_encoder(const std::string &temp_dir, compression_params &cp);

    std::string random_string(size_t length);

}  // namespace spring

#endif  // SPRING_SPRING_H_
