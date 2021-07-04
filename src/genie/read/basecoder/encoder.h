/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_BASECODER_ENCODER_H_
#define SRC_GENIE_READ_BASECODER_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

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
 *
 */
class Encoder {
 private:
    core::AccessUnit container;  //!<
    int32_t pos;                 //!<
    uint32_t readCounter;        //!<

    /**
     *
     * @param rec
     */
    void encodeFirstSegment(const core::record::Record &rec);

    /**
     *
     * @param length
     * @param srec
     */
    void encodeAdditionalSegment(size_t length, const core::record::alignment_split::SameRec &srec, bool first1);

    /**
     *
     */
    struct ClipInformation {
        std::array<std::string, 2> softClips;        //!<
        std::array<size_t, 2> hardClips = {{0, 0}};  //!<
    };

    /**
     *
     * @param read
     * @param cigar
     * @param ref
     * @param type
     * @return
     */
    ClipInformation encodeCigar(const std::string &read, const std::string &cigar, const std::string &ref,
                                core::record::ClassType type);

    /**
     *
     * @param inf
     * @param last
     * @return
     */
    bool encodeSingleClip(const ClipInformation &inf, bool last);

    /**
     *
     * @param clips
     */
    void encodeClips(const std::pair<ClipInformation, ClipInformation> &clips);

    /**
     *
     */
    struct CodingState {
        /**
         *
         * @param _read
         * @param _ref
         * @param _type
         */
        CodingState(const std::string &_read, const std::string &_ref, core::record::ClassType _type);
        size_t count;         //!<
        size_t read_pos;      //!<
        size_t ref_offset;    //!<
        size_t lastMisMatch;  //!<
        bool isRightClip;     //!<

        const std::string &read;             //!<
        const std::string &ref;              //!<
        const core::record::ClassType type;  //!<
        ClipInformation clips;               //!<
    };

    /**
     *
     * @param state
     */
    void encodeInsertion(CodingState &state);

    /**
     *
     * @param state
     */
    void encodeDeletion(CodingState &state);

    /**
     *
     * @param state
     */
    static void encodeHardClip(CodingState &state);

    /**
     *
     * @param state
     */
    static void encodeSoftClip(CodingState &state);

    /**
     *
     * @param state
     */
    void encodeSubstitution(CodingState &state);

    /**
     *
     * @param state
     */
    void encodeSplice(CodingState &state);

    /**
     *
     * @param state
     */
    void encodeMatch(CodingState &state);

    /**
     *
     * @param cigar_char
     * @param state
     * @return
     */
    static bool updateCount(char cigar_char, CodingState &state);

    /**
     *
     * @param cigar_char
     * @param state
     */
    void encodeCigarToken(char cigar_char, CodingState &state);

    /**
     *
     * @param rec
     * @return
     */
    const core::record::alignment_split::SameRec &extractPairedAlignment(const core::record::Record &rec) const;

 public:
    /**
     *
     * @param startingMappingPos
     */
    explicit Encoder(int32_t startingMappingPos);

    /**
     *
     * @param rec
     * @param ref1
     * @param ref2
     */
    void add(const core::record::Record &rec, const std::string &ref1, const std::string &ref2);

    /**
     *
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
