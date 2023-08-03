/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ENTROPY_JBIG_ENCODER_H
#define GENIE_ENTROPY_JBIG_ENCODER_H

#include <sstream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace jbig {

// ---------------------------------------------------------------------------------------------------------------------

class Encoder {
 private:

 public:
    void encode(std::stringstream &input, std::stringstream &output);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace jbig
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENTROPY_JBIG_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
