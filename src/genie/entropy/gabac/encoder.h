/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_ENCODER_H_
#define SRC_GENIE_ENTROPY_GABAC_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/access_unit.h"
#include "genie/core/entropy_encoder.h"
#include "genie/entropy/gabac/gabac-seq-conf-set.h"
#include "genie/entropy/gabac/gabac.h"
#include "genie/util/make_unique.h"
#include "genie/util/stop_watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 * @brief Module to compress raw access units into blockpayloads using GABAC
 */
class Encoder : public core::EntropyEncoder {
 private:
    /**
     * @brief Run the actual gabac compression
     * @param conf GABAC configuration to use
     * @param in Uncompressed data
     * @param out Where to put compressed data. A set of transformed subsequences is generated
     */
    static core::AccessUnit::Subsequence compress(const gabac::EncodingConfiguration& conf,
                                                  core::AccessUnit::Subsequence&& in);

    /**
     * @brief
     * @param conf0
     * @param in
     * @return
     */
    static core::AccessUnit::Descriptor compressTokens(const gabac::EncodingConfiguration& conf0,
                                                       core::AccessUnit::Descriptor&& in);

 public:
    GabacSeqConfSet configSet;  //!< @brief Config set to use. In contrast to decompression it's static over time
    bool writeOutStreams{};     //!< @brief writeout streams

    /**
     * @brief
     * @param desc
     * @return
     */
    EntropyCoded Process(core::AccessUnit::Descriptor& desc) override;

    /**
     * @brief
     * @param _writeOutStreams
     */
    explicit Encoder(bool _writeOutStreams);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// -----------------------zzzz----------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
