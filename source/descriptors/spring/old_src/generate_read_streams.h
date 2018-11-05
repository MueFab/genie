#ifndef SPRING_GENERATE_READ_STREAMS_H_
#define SPRING_GENERATE_READ_STREAMS_H_

#include <string>
#include "descriptors/spring/util.h"

namespace spring {

void generate_read_streams_se(const std::string &temp_dir,
                              const compression_params &cp);

void generate_read_streams_pe(const std::string &temp_dir,
                              const compression_params &cp);
}  // namespace spring

#endif  // SPRING_GENERATE_READ_STREAMS_H_
