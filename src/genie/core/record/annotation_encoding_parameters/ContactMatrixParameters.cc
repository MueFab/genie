/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#include "ContactMatrixParameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_encoding_parameters {

    
void ContactMatrixParameters::read(util::BitReader& reader) {
    num_samples = static_cast<uint8_t>(reader.read_b(8));
    
    for (auto i = 0; i < num_samples; ++i) {
        sample_ID.push_back(static_cast<uint8_t>(reader.read_b(8)));
        std::string name;
        reader.readBypass(name);
        sample_name.push_back(name);
    }

    num_chrs = reader.readBypassBE<uint8_t>();
    for (auto i = 0; i < num_chrs; ++i) {
        chr_ID.push_back(static_cast<uint8_t>(reader.read_b(8)));
        std::string name;
        reader.readBypass(name);
        chr_name.push_back(name);
        chr_length.push_back(reader.readBypassBE<uint64_t>());
    }

    interval = reader.readBypassBE<uint32_t>();
    tile_size = reader.readBypassBE<uint32_t>();

    num_interval_multipliers = static_cast<uint8_t>(reader.read_b(8));
    for (auto i = 0; i < num_interval_multipliers; ++i) interval_multiplier.push_back(reader.readBypassBE<uint32_t>());

    num_norm_methods = static_cast<uint8_t>(reader.read_b(8));
    for (auto i = 0; i < num_norm_methods; ++i) {
        norm_method_ID.push_back(static_cast<uint8_t>(reader.read_b(8)));
        std::string name;
        reader.readBypass(name);
        chr_name.push_back(name);
        reader.readBypass(name);
        norm_method_name.push_back(name);
        norm_method_mult_flag.push_back(static_cast<bool>(reader.read_b(1)));
    }

    num_norm_matrices = static_cast<uint8_t>(reader.read_b(8));
    for (auto i = 0; i < num_norm_matrices; ++i) {
        norm_matrix_ID.push_back(static_cast<uint8_t>(reader.read_b(8)));
        std::string name;
        reader.readBypass(name);
        norm_matrix_name.push_back(name);
    }
}


// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_encoding_parameters
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
