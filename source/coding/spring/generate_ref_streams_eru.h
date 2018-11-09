#ifndef SPRING_GENERATE_REF_STREAMS_ERU_H_
#define SPRING_GENERATE_REF_STREAMS_ERU_H_

#include <string>
#include <vector>
#include <map>

#include "spring/util.h"
#include "ReturnStructures.h"

namespace spring {

generated_aus_ref generate_ref_streams(const std::string &temp_dir, const compression_params &cp);

}  // namespace spring

#endif // SPRING_GENERATE_REF_STREAMS_ERU_H_
