/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <memory>
 
#include "genie/entropy/factory/encoder_factory.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/ser/encoder.h"
#include "genie/entropy/zstd/encoder.h"

/// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace factory {

std::unique_ptr<base::Encoder> EncoderFactory::createEncoder(
    genie::core::AlgoID algorithmID,
    const genie::core::record::annotation_parameter_set::AlgorithmParameters& parameters) {

    switch (algorithmID) {
        case genie::core::AlgoID::BSC: {
            auto encoder = std::make_unique<bsc::BSCEncoder>();
            // Configure encoder with parameters if needed
            return encoder;
        }
        case genie::core::AlgoID::LZMA: {
            auto encoder = std::make_unique<lzma::LZMAEncoder>();
            // Configure encoder with parameters if needed
            return encoder;
        }
        case genie::core::AlgoID::ZSTD: {
            auto encoder = std::make_unique<zstd::ZSTDEncoder>();
            // Configure encoder with parameters if needed
            return encoder;
        }
        case genie::core::AlgoID::SER: {
            auto encoder = std::make_unique<ser::SEREncoder>();
            // Configure encoder with parameters if needed
            return encoder;
        }
        default:
            throw std::runtime_error("Unsupported algorithm ID");
    }
}

}  // namespace factory
}  // namespace entropy
}  // namespace genie
