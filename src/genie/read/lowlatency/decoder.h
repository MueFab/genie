/**
 * @file lowlatency_decoder.h
 * @brief Header file for the `Decoder` class in the `lowlatency` namespace.
 *
 * This file defines the `Decoder` class for low-latency decoding in the GENIE framework.
 * It provides methods to decode access units into sequences and handle the decoding
 * of records in a low-latency manner, facilitating efficient stream-based processing.
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_LOWLATENCY_DECODER_H_
#define SRC_GENIE_READ_LOWLATENCY_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <limits>
#include <string>
#include "genie/core/read-decoder.h"
#include "genie/core/ref-decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::lowlatency {

/**
 * @brief Low-latency decoder for handling access units in the GENIE framework.
 *
 * This class implements a low-latency decoder that inherits from both `ReadDecoder` and `RefDecoder`.
 * It provides functionality to decode incoming access units into raw sequences or chunks of records,
 * enabling efficient low-latency decoding operations in read handling.
 */
class Decoder : public core::ReadDecoder, public core::RefDecoder {
 private:
 public:
    /**
     * @brief Decodes the incoming access unit into a raw sequence.
     * @param t The access unit to decode.
     * @return The decoded sequence as a string.
     */
    std::string decode(core::AccessUnit&& t) override;

    /**
     * @brief Decodes the incoming access unit into a record chunk.
     * @param t The access unit to decode.
     * @return The decoded chunk of records.
     */
    core::record::Chunk decode_common(core::AccessUnit&& t);

    /**
     * @brief Processes the incoming access unit and passes it to the appropriate downstream components.
     * @param t The access unit to decode.
     * @param id The multithreading section identifier.
     */
    void flowIn(core::AccessUnit&& t, const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::lowlatency

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_LOWLATENCY_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
