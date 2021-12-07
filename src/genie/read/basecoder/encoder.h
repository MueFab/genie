/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_BASECODER_ENCODER_H_
#define SRC_GENIE_READ_BASECODER_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/read-encoder.h>
#include <genie/util/watch.h>
#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/access-unit.h"
#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/core/record/record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace basecoder {

/**
 * @brief
 */
class Encoder {
 private:
    core::AccessUnit container;  //!< @brief
    int32_t pos;                 //!< @brief
    uint32_t readCounter;        //!< @brief

    /**
     * @brief
     * @param rec
     */
    void encodeFirstSegment(const core::record::Record &rec);

    /**
     * @brief
     * @param length
     * @param srec
     */
    void encodeAdditionalSegment(size_t length, const core::record::alignment_split::SameRec &srec, bool first1);

    /**
     * @brief
     */
    struct ClipInformation {
        std::array<std::string, 2> softClips;        //!< @brief
        std::array<size_t, 2> hardClips = {{0, 0}};  //!< @brief
    };

    /**
     * @brief
     * @param read
     * @param cigar
     * @param ref
     * @param type
     * @return
     */
    ClipInformation encodeCigar(const std::string &read, const std::string &cigar, const std::string &ref,
                                core::record::ClassType type);

    /**
     * @brief
     * @param inf
     * @param last
     * @return
     */
    bool encodeSingleClip(const ClipInformation &inf, bool last);

    /**
     * @brief
     * @param clips
     */
    void encodeClips(const std::pair<ClipInformation, ClipInformation> &clips);

    /**
     * @brief
     */
    struct CodingState {
        /**
         * @brief
         * @param _read
         * @param _ref
         * @param _type
         */
        CodingState(const std::string &_read, const std::string &_ref, core::record::ClassType _type);
        size_t count;         //!< @brief
        size_t read_pos;      //!< @brief
        size_t ref_offset;    //!< @brief
        size_t lastMisMatch;  //!< @brief
        bool isRightClip;     //!< @brief

        const std::string &read;             //!< @brief
        const std::string &ref;              //!< @brief
        const core::record::ClassType type;  //!< @brief
        ClipInformation clips;               //!< @brief
    };

    /**
     * @brief
     * @param state
     */
    void encodeInsertion(CodingState &state);

    /**
     * @brief
     * @param state
     */
    void encodeDeletion(CodingState &state);

    /**
     * @brief
     * @param state
     */
    static void encodeHardClip(CodingState &state);

    /**
     * @brief
     * @param state
     */
    static void encodeSoftClip(CodingState &state);

    /**
     * @brief
     * @param state
     */
    void encodeSubstitution(CodingState &state);

    /**
     * @brief
     * @param state
     */
    void encodeSplice(CodingState &state);

    /**
     * @brief
     * @param state
     */
    void encodeMatch(CodingState &state);

    /**
     * @brief
     * @param cigar_char
     * @param state
     * @return
     */
    static bool updateCount(char cigar_char, CodingState &state);

    /**
     * @brief
     * @param cigar_char
     * @param state
     */
    void encodeCigarToken(char cigar_char, CodingState &state);

    /**
     * @brief
     * @param rec
     * @return
     */
    const core::record::alignment_split::SameRec &extractPairedAlignment(const core::record::Record &rec) const;

 public:
    /**
     * @brief
     * @param startingMappingPos
     */
    explicit Encoder(int32_t startingMappingPos);

    /**
     * @brief
     * @param rec
     * @param ref1
     * @param ref2
     */
    void add(const core::record::Record &rec, const std::string &ref1, const std::string &ref2);

    /**
     * @brief
     * @return
     */
    core::AccessUnit &&moveStreams();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace basecoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_BASECODER_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
