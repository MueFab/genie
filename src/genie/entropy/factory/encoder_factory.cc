/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "encoder_factory.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/ser/encoder.h"
#include "genie/entropy/zstd/encoder.h"

/// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace factory {

std::unique_ptr<base::Encoder> EncoderFactory::createEncoder(
    AlgorithmID algorithmID,
    const annotation_parameter_set::AlgorithmParameters& parameters) {
    
    switch (algorithmID) {
        case AlgorithmID::BSC: {
            auto encoder = std::make_unique<bsc::BSCEncoder>();
            // Configure encoder with parameters if needed
            return encoder;
        }
        case AlgorithmID::LZMA: {
            auto encoder = std::make_unique<lzma::LZMAEncoder>();
            // Configure encoder with parameters if needed
            return encoder;
        }
        case AlgorithmID::ZSTD: {
            auto encoder = std::make_unique<zstd::ZSTDEncoder>();
            // Configure encoder with parameters if needed
            return encoder;
        }
        //case AlgorithmID::JBIG: {
        //    auto encoder = std::make_unique<jbig::JBIGEncoder>();
        //    // Configure encoder with parameters if needed
        //    return encoder;
        //}
        case AlgorithmID::RLE: {
            auto encoder = std::make_unique<rle::RLEEncoder>();
            // Configure encoder with parameters if needed
            return encoder;
        }
        case AlgorithmID::SER: {
            auto encoder = std::make_unique<ser::SEREncoder>();
            // Configure encoder with parameters if needed
            return encoder;
        }
        default:
            throw std::runtime_error("Unsupported algorithm ID");
    }
}

} // namespace factory
} // namespace entropy
} // namespace genie