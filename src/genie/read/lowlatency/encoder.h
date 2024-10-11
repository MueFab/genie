/**
 * @file lowlatency_encoder.h
 * @brief Header file for the `Encoder` class in the `read::lowlatency` namespace.
 *
 * This file defines the `Encoder` class for low-latency encoding in the GENIE framework.
 * The `Encoder` class provides functionality for handling and encoding chunks of genomic records into access units,
 * supporting low-latency processing and management of genomic data.
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_LOWLATENCY_ENCODER_H_
#define SRC_GENIE_READ_LOWLATENCY_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "genie/core/read-encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::lowlatency {

/**
 * @brief Low-latency encoder for genomic records.
 *
 * The `Encoder` class implements a low-latency encoder that inherits from the `ReadEncoder` class.
 * It provides methods to process incoming chunks of genomic records, encode them into access units,
 * and manage the state of encoding operations in a low-latency environment.
 */
class Encoder : public core::ReadEncoder {
 private:
    /**
     * @brief Internal state structure for managing the low-latency encoding process.
     */
    struct LLState {
        size_t readLength;         //!< @brief Length of the read being processed.
        bool pairedEnd;            //!< @brief Indicates whether the reads are paired-end.
        core::AccessUnit streams;  //!< @brief Encoded data streams for the current read.
        bool refOnly;              //!< @brief Flag indicating if only reference data is being processed.
    };

 public:
    /**
     * @brief Processes an incoming chunk of records and encodes them into an access unit.
     *
     * The `flowIn` method is used to handle incoming chunks of records. It processes the records,
     * encodes them into access units, and manages the state of the low-latency encoding process.
     *
     * @param t The chunk of records to encode.
     * @param id The multithreading section identifier.
     */
    void flowIn(core::record::Chunk&& t, const util::Section& id) override;

    /**
     * @brief Packs the encoded streams into an access unit with the specified parameters.
     *
     * The `pack` method assembles the encoded streams into a complete access unit, using the provided
     * quality value depth, quality value parameters, and the state of the encoder.
     *
     * @param id The multithreading section identifier.
     * @param qv_depth Depth of the quality value encoding.
     * @param qvparam Pointer to the quality value parameters.
     * @param state The current state of the encoder.
     * @return Assembled access unit with the provided parameters.
     */
    static core::AccessUnit pack(const util::Section& id, uint8_t qv_depth,
                                 std::unique_ptr<core::parameter::QualityValues> qvparam, LLState& state);

    /**
     * @brief Constructs the encoder with the specified raw writing mode.
     *
     * Initializes the `Encoder` with the specified raw writing mode, which determines
     * whether raw data should be written without compression.
     *
     * @param write_raw If set to true, raw data will be written without compression.
     */
    explicit Encoder(bool write_raw) : core::ReadEncoder(write_raw) {}
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::lowlatency

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_LOWLATENCY_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
