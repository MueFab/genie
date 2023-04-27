/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_CONTACTMATRIXPARAMETERS_H_
#define SRC_GENIE_CORE_RECORD_CONTACTMATRIXPARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_encoding_parameters {

class ContactMatrixParameters {
 private:
    uint8_t num_samples{};
    std::vector<uint8_t> sample_ID{};
    std::vector<std::string> sample_name{};
    uint8_t num_chrs{};
    std::vector<uint8_t> chr_ID{};
    std::vector<std::string> chr_name{};
    std::vector<uint64_t> chr_length{};

    uint32_t interval{};
    uint32_t tile_size{};
    uint8_t num_interval_multipliers{};
    std::vector<uint32_t> interval_multiplier{};

    uint8_t num_norm_methods{};
    std::vector<uint8_t> norm_method_ID{};
    std::vector<std::string> norm_method_name{};
    std::vector<bool> norm_method_mult_flag{};

    uint8_t num_norm_matrices{};
    std::vector<uint8_t> norm_matrix_ID{};
    std::vector<std::string> norm_matrix_name{};

 public:
    void read(util::BitReader& reader);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_encoding_parameters
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_CONTACTMATRIXPARAMETERS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
