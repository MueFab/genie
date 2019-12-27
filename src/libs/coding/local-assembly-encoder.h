/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_LOCAL_ASSEMBLY_ENCODER_H
#define GENIE_LOCAL_ASSEMBLY_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <format/mpegg_rec/split_alignment/split-alignment-same-rec.h>
#include "mpegg-encoder.h"
#include "read-encoder.h"
#include "reference-encoder.h"

/**
 * @brief Module using local assembly to encode aligned reads
 */
class LocalAssemblyEncoder : public MpeggEncoder {
   private:
    bool debug;                //!< @brief If true, debugging information will be printed to std::cout
    uint32_t cr_buf_max_size;  //!< @brief Buffer size for local assembly reference memory

    /**
     * @brief Internal local assembly state (not exposed publicly)
     */
    struct LaeState {
        /**
         * @brief Construct internal variables with default values
         * @param cr_buf_max_size Buffer size for local assembly reference memory
         */
        explicit LaeState(size_t cr_buf_max_size);
        lae::LocalAssemblyReferenceEncoder refCoder;          //!< @brief Building the local reference
        lae::LocalAssemblyReadEncoder readCoder;              //!< @brief Generating the descriptor streams
        bool pairedEnd;                                       //!< @brief Current guess regarding pairing
        size_t readLength;                                    //!< @brief Current guess regarding read length
        format::mpegg_rec::MpeggRecord::ClassType classType;  //!< @brief Current guess regarding class type
    };

    /**
     * @brief Print debug information to the terminal
     * @param state Current local assembly state
     * @param ref1 Reference sequence for the first aligned record in a pair
     * @param ref2 Reference sequence for second record in a pair, if any
     * @param r The record we are currently processing
     */
    void printDebug(const LocalAssemblyEncoder::LaeState& state, const std::string& ref1, const std::string& ref2,
                    const format::mpegg_rec::MpeggRecord& r) const;

    /**
     * @brief Updates the assembly itself. The local reference will be updated and descriptor streams extended
     * @param r The record we will be updating with
     * @param srec Paired alignment extracted from the record
     * @param state Current local assembly state
     */
    void updateAssembly(const format::mpegg_rec::MpeggRecord& r, const format::mpegg_rec::SplitAlignmentSameRec& srec,
                        LocalAssemblyEncoder::LaeState* state) const;

    /**
     * @brief Checks if there is an additional alignment available and converts it into the correct format
     * @param state Current local assembly state
     * @param r Current record
     * @return Preprocessed alignment for pair
     */
    const format::mpegg_rec::SplitAlignmentSameRec* getPairedAlignment(const LocalAssemblyEncoder::LaeState& state,
                                                                       const format::mpegg_rec::MpeggRecord& r) const;

    /**
     * @brief Update guesses regarding read length, pairing ...
     * @param state Current local assembly state
     * @param r Current input record
     */
    void updateGuesses(LocalAssemblyEncoder::LaeState* state, const format::mpegg_rec::MpeggRecord& r) const;

    /**
     * @brief Generate a full MpeggRawAu and parameter set as a last step of encoding
     * @param id Block identifier for the current data
     * @param state The state used throughout the current encoding
     * @return MpeggRawAu ready for entropy coding
     */
    std::unique_ptr<MpeggRawAu> pack(size_t id, LocalAssemblyEncoder::LaeState* state) const;

   public:
    /**
     * @brief Process one chunk of data
     * @param t The input data
     * @param id The current block identifier
     */
    void flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> t, size_t id) override;

    /**
     * @brief Propagate end-of-data signal.
     */
    void dryIn() override;

    /**
     * @brief Initialize the module
     * @param _cr_buf_max_size Buffer size for local assembly reference memory. Will end up in parameter set
     * @param _debug If additional debugging information shall be printed
     */
    LocalAssemblyEncoder(uint32_t _cr_buf_max_size, bool _debug);
};

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_LOCAL_ASSEMBLY_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------