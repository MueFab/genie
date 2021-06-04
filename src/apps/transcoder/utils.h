#ifndef GENIE_TRANSCODER_UTILS_H
#define GENIE_TRANSCODER_UTILS_H

#include <genie/core/record/record.h>

namespace genie {
namespace transcoder {

uint64_t getMinPos(const genie::core::record::Record& r);

bool compare(const genie::core::record::Record& r1,
             const genie::core::record::Record& r2);

}
}

#endif  // GENIE_TRANSCODER_UTILS_H
