/**
 * @file decoderstub.h
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for the basecoder `DecoderStub` class.
 *
 * This file defines the `DecoderStub` class, which serves as a common base for
 * various aligned record decoders in the GENIE framework. It provides a foundation
 * for implementing custom decoding logic by handling common operations like
 * sequence, name, and quality value decoding.
 *
 * @details The `DecoderStub` class interacts with the core modules of the GENIE
 * framework, such as the `core::AccessUnit` and `core::record::Chunk`, to manage
 * the flow of input data and to support customized decoding logic. Subclasses
 * can override specific member functions to implement tailored behavior.
 */

#ifndef SRC_GENIE_READ_BASECODER_DECODERSTUB_H_
#define SRC_GENIE_READ_BASECODER_DECODERSTUB_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <limits>
#include <memory>
#include <string>
#include <vector>
#include "genie/core/read-decoder.h"
#include "genie/read/basecoder/decoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::basecoder {

/**
 * @brief Common stub for all aligned record decoders.
 *
 * The `DecoderStub` class provides a common interface and implementation
 * foundation for decoders handling aligned genomic records. It defines methods
 * for sequence, name, and quality value decoding, and includes hooks for
 * additional processing or customization in subclasses.
 */
class DecoderStub : public core::ReadDecoder {
 protected:
    /**
     * @brief Internal state for decoding operations.
     *
     * The `DecodingState` structure holds intermediate data required for decoding,
     * including quality values, record positions, and CIGAR strings.
     */
    struct DecodingState {
        core::AccessUnit::Descriptor qvStream;                    //!< @brief Quality value descriptor.
        core::AccessUnit::Descriptor nameStream;                  //!< @brief Name stream descriptor.
        size_t numRecords;                                        //!< @brief Number of records to decode.
        size_t numSegments;                                       //!< @brief Number of template segments.
        size_t ref;                                               //!< @brief Reference identifier.
        std::vector<std::string> ecigars;                         //!< @brief Vector of CIGAR strings.
        std::vector<uint64_t> positions;                          //!< @brief Vector of reference positions.
        std::unique_ptr<core::parameter::QualityValues> qvparam;  //!< @brief Quality value parameters.

        /**
         * @brief Constructs a `DecodingState` from an access unit.
         * @param t_data The access unit to decode.
         */
        explicit DecodingState(core::AccessUnit& t_data);

        /**
         * @brief Destructor for `DecodingState`.
         */
        virtual ~DecodingState() = default;
    };

    /**
     * @brief Creates a new decoding state.
     * @param t Access unit to initialize the decoding state.
     * @return Pointer to the newly created `DecodingState`.
     */
    virtual std::unique_ptr<DecodingState> createDecodingState(core::AccessUnit& t) = 0;

    /**
     * @brief Gets reference sequences for a segment based on metadata.
     * @param meta Segment metadata.
     * @param state Current decoding state.
     * @return Vector of reference sequences.
     */
    virtual std::vector<std::string> getReferences(const basecoder::Decoder::SegmentMeta& meta,
                                                   DecodingState& state) = 0;

    /**
     * @brief Decodes sequences from the access unit.
     * @param state Current decoding state.
     * @param t_data Access unit containing sequence data.
     * @return Decoded chunk of records.
     */
    virtual core::record::Chunk decodeSequences(DecodingState& state, core::AccessUnit& t_data);

    /**
     * @brief Decodes read names from the access unit.
     * @param state Current decoding state.
     * @param chunk The chunk of records to populate with names.
     */
    virtual void decodeNames(DecodingState& state, core::record::Chunk& chunk);

    /**
     * @brief Decodes quality values from the access unit.
     * @param state Current decoding state.
     * @param chunk The chunk of records to populate with quality values.
     */
    virtual void decodeQualities(DecodingState& state, core::record::Chunk& chunk);

    /**
     * @brief Adds an extended CIGAR string to the record.
     * @param rec The record to modify.
     * @param cig_vec Vector to store CIGAR strings.
     * @param pos_vec Vector to store positions.
     */
    virtual void addECigar(const core::record::Record& rec, std::vector<std::string>& cig_vec,
                           std::vector<uint64_t>& pos_vec);

    /**
     * @brief Hook for processing after a record has been decoded.
     * @param state Current decoding state.
     * @param rec The record that was decoded.
     */
    virtual void recordDecodedHook(DecodingState&, const core::record::Record&) {}

 public:
    /**
     * @brief Processes an incoming access unit and decodes it.
     * @param t The access unit to decode.
     * @param id Section identifier for the access unit.
     */
    void flowIn(core::AccessUnit&& t, const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::basecoder

#endif  // SRC_GENIE_READ_BASECODER_DECODERSTUB_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
