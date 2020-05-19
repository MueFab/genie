/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_LA_ENCODER_H
#define GENIE_LA_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/read-encoder.h>
#include <genie/core/record/alignment_split/same-rec.h>
#include <genie/read/basecoder/encoder.h>
#include "local-reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace localassembly {

/**
 * @brief Module using local assembly to encode aligned reads
 */
class Encoder : public core::ReadEncoder {
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
         * @param startingPos Starting position of access unit
         */
        explicit LaeState(size_t cr_buf_max_size, uint64_t startingPos);
        LocalReference refCoder;            //!< @brief Building the local reference
        basecoder::Encoder readCoder;       //!< @brief Generating the descriptor streams
        bool pairedEnd;                     //!< @brief Current guess regarding pairing
        size_t readLength;                  //!< @brief Current guess regarding read length
        core::record::ClassType classType;  //!< @brief Current guess regarding class type
        uint64_t minPos;                    //!<
        uint64_t maxPos;                    //!<
    };

    /**
     * @brief Print debug information to the terminal
     * @param state Current local assembly state
     * @param ref1 Reference sequence for the first aligned record in a pair
     * @param ref2 Reference sequence for second record in a pair, if any
     * @param r The record we are currently processing
     */
    void printDebug(const Encoder::LaeState& state, const std::string& ref1, const std::string& ref2,
                    const core::record::Record& r) const;

    /**
     * @brief Updates the assembly itself. The local reference will be updated and descriptor streams extended
     * @param r The record we will be updating with
     * @param srec Paired alignment extracted from the record
     * @param state Current local assembly state
     */
    void updateAssembly(const core::record::Record& r, Encoder::LaeState& state) const;

    /**
     * @brief Checks if there is an additional alignment available and converts it into the correct format
     * @param state Current local assembly state
     * @param r Current record
     * @return Preprocessed alignment for pair
     */
    const core::record::alignment_split::SameRec& getPairedAlignment(const Encoder::LaeState& state,
                                                                     const core::record::Record& r) const;

    /**
     * @brief Update guesses regarding read length, pairing ...
     * @param state Current local assembly state
     * @param r Current input record
     */
    void updateGuesses(const core::record::Record& r, Encoder::LaeState& state) const;

    /**
     * @brief Generate a full RawAccessUnit and parameter set as a last step of encoding
     * @param id Block identifier for the current data
     * @param state The state used throughout the current encoding
     * @return RawAccessUnit ready for entropy core
     */
    core::AccessUnitRaw pack(size_t id, uint16_t ref, uint8_t qv_depth,
                             std::unique_ptr<core::parameter::QualityValues> qvparam, core::record::ClassType type,
                             Encoder::LaeState& state) const;

   public:
    /**
     * @brief Process one chunk of data
     * @param t The input data
     * @param id The current block identifier
     */
    void flowIn(core::record::Chunk&& t, const util::Section& id) override;

    /**
     * @brief Initialize the module
     * @param _cr_buf_max_size Buffer size for local assembly reference memory. Will end up in parameter set
     * @param _debug If additional debugging information shall be printed
     */
    Encoder(uint32_t _cr_buf_max_size, bool _debug);

    /**
     *
     * @param cigar
     * @return
     */
    static uint64_t getLengthOfCigar(const std::string& cigar);

    /**
     *
     * @param position
     * @param cigar
     * @param state
     */
    void updateAUBoundaries(uint64_t position, const std::string& cigar, LaeState& state) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace localassembly
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------