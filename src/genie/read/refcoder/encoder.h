/**
 * @file refcoder_encoder.h
 * @brief Header file for the `Encoder` class in the `read::refcoder` namespace.
 *
 * This file defines the `Encoder` class used for encoding genomic records using reference-based encoding.
 * The `Encoder` class utilizes a reference sequence to efficiently encode aligned reads,
 * creating a compact representation of the data for storage or transmission.
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_REFCODER_ENCODER_H_
#define SRC_GENIE_READ_REFCODER_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/stop-watch.h>
#include <memory>
#include <string>
#include <utility>
#include "genie/core/read-encoder.h"
#include "genie/read/basecoder/encoderstub.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::refcoder {

/**
 * @brief Reference-based encoder for aligned genomic reads.
 *
 * The `Encoder` class implements the encoding of genomic reads using a reference sequence.
 * It extends the `basecoder::EncoderStub` class to add specific logic for managing reference excerpts,
 * encoding metadata, and utilizing reference information for compact data representation.
 */
class Encoder : public basecoder::EncoderStub {
 private:
    /**
     * @brief Internal state for managing reference-based encoding.
     *
     * The `RefEncodingState` structure stores the reference excerpt and other state variables needed
     * for encoding records using the reference sequence. This state is maintained across multiple records
     * in a chunk to facilitate efficient encoding.
     */
    struct RefEncodingState : public EncoderStub::EncodingState {
        /**
         * @brief Constructs the state from a chunk of data.
         *
         * Initializes the reference excerpt and other state variables from the given chunk of records,
         * preparing for the encoding of reads using reference information.
         *
         * @param data The chunk of records to initialize the state from.
         */
        explicit RefEncodingState(const core::record::Chunk& data);

        core::ReferenceManager::ReferenceExcerpt
            excerpt;  //!< @brief Reference information for managing sequence encoding.
    };

    /**
     * @brief Retrieves reference sequences for a given record.
     *
     * The `getReferences` method extracts the relevant reference sequences for the specified record using
     * the reference excerpt stored in the current encoding state. This method is called during the encoding
     * process to obtain the reference sequences needed for aligned read encoding.
     *
     * @param r The record for which reference sequences are needed.
     * @param state The current encoding state containing reference information.
     * @return A pair of reference sequences. If the record is unpaired, the second reference will be empty.
     */
    std::pair<std::string, std::string> getReferences(const core::record::Record& r, EncodingState& state) override;

    /**
     * @brief Creates a new encoding state from the provided chunk of data.
     *
     * The `createState` method initializes the `RefEncodingState` for the current encoding process
     * based on the incoming chunk of records. It prepares the necessary reference data for subsequent
     * encoding operations.
     *
     * @param data The chunk of records to initialize the encoding state from.
     * @return A unique pointer to the newly created encoding state.
     */
    [[nodiscard]] std::unique_ptr<EncodingState> createState(const core::record::Chunk& data) const override;

 public:
    /**
     * @brief Constructs the `Encoder` object with the specified configuration.
     *
     * @param raw_write A boolean flag indicating whether to write raw streams in the output.
     */
    explicit Encoder(bool raw_write) : basecoder::EncoderStub(raw_write) {}
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::refcoder

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_REFCODER_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
