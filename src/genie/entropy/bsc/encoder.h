/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_BSC_ENCODER_H_
#define SRC_GENIE_ENTROPY_BSC_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/access-unit.h"
#include "genie/core/entropy-encoder.h"
#include "genie/core/parameter/descriptor_present/decoder-regular.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::bsc {

/**
 * @brief Module to compress raw access units into blockpayloads using GABAC
 */
class Encoder : public core::EntropyEncoder {
 public:
    bool writeOutStreams{};  //!< @brief writeout streams

    /**
     * @brief
     * @param desc
     * @return
     */
    EntropyCoded process(core::AccessUnit::Descriptor &desc) override;

    /**
     * @brief
     * @param _writeOutStreams
     */
    explicit Encoder(bool _writeOutStreams);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_BSC_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
