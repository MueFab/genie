/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_LOCALASSEMBLY_ENCODER_H_
#define SRC_GENIE_READ_LOCALASSEMBLY_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include "genie/core/read_encoder.h"
#include "genie/core/record/alignment_split/same_rec.h"
#include "genie/read/basecoder/encoderstub.h"
#include "genie/read/localassembly/local-reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace localassembly {

/**
 * @brief Module using a local computed reference to encode records
 */
class Encoder : public basecoder::EncoderStub {
 private:
    bool debug;  //!< @brief print debug output

    /**
     * @brief Internal encoding state (not exposed publicly)
     */
    struct LAEncodingState : public basecoder::EncoderStub::EncodingState {
        /**
         * @brief Construct form raw data
         * @param data Chunk of data to construct from
         * @param _cr_buf_max_size  LA parameter
         */
        explicit LAEncodingState(const core::record::Chunk& data, uint32_t _cr_buf_max_size);
        LocalReference refCoder;  //!< @brief Local assembly
    };
    /**
     * @brief Print current local assembly state
     * @param state Current state
     * @param ref1 Reference of first segment
     * @param ref2 Reference of second segment
     * @param r Current record
     */
    void printDebug(const LAEncodingState& state, const std::string& ref1, const std::string& ref2,
                    const core::record::Record& r) const;

    /**
     * @brief Pack encoded data into an access unit
     * @param id Multithreading id
     * @param qv Encoded quality values
     * @param rname Encoded record names
     * @param state Encoding state
     * @return Access unit with parameters
     */
    core::AccessUnit pack(size_t id, core::QvEncoder::qv_coded qv, core::AccessUnit::Descriptor rname,
                          EncodingState& state) override;

    /**
     * @brief Calculate references for record from local assembly
     * @param r Record to calculate for
     * @param state Encoding state
     * @return Pair of references. If unpaired, second ref remains empty.
     */
    std::pair<std::string, std::string> getReferences(const core::record::Record& r, EncodingState& state) override;

    /**
     * @brief Create a new encoding state form chunk of data
     * @param data Chunk to create from
     * @return LAEncodingState
     */
    std::unique_ptr<EncodingState> createState(const core::record::Chunk& data) const override;

 public:
    /**
     * @brief Create encoder with specified parameters
     * @param _debug If true, debug information is printed
     * @param _write_raw
     */
    Encoder(bool _debug, bool _write_raw);

    /**
     * @brief Downgrade class N to class M and then proceed with encoding
     * @param t Data chunk
     * @param id Multithreading id
     */
    void FlowIn(core::record::Chunk&& t, const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace localassembly
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_LOCALASSEMBLY_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
