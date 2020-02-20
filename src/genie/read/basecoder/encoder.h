/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_R_ENCODER_H
#define GENIE_R_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>

#include <genie/core/access-unit-raw.h>
#include <genie/core/record/alignment_split/same-rec.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace basecoder {
/**
 *
 */
class Encoder {
   private:
    core::AccessUnitRaw container;
    int32_t pos;
    uint32_t readCounter;

    void encodeFirstSegment(const core::record::Record &rec);

    void encodeAdditionalSegment(size_t length, const core::record::alignment_split::SameRec &srec);

    struct ClipInformation {
        std::array<std::string, 2> softClips;
        std::array<size_t, 2> hardClips = {0, 0};
    };

    ClipInformation encodeCigar(const std::string &read, const std::string &cigar, const std::string &ref,
                                core::record::ClassType type);

    bool encodeSingleClip(const ClipInformation &inf, bool last);

    void encodeClips(const std::pair<ClipInformation, ClipInformation> &clips);

    struct CodingState {
        CodingState(const std::string &_read, const std::string &_ref, core::record::ClassType _type);
        size_t count;
        size_t read_pos;
        size_t ref_offset;
        size_t lastMisMatch;
        bool isRightClip;

        const std::string &read;
        const std::string &ref;
        const core::record::ClassType type;
        ClipInformation clips;
    };

    void encodeInsertion(CodingState &state);

    void encodeDeletion(CodingState &state);

    static void encodeHardClip(CodingState &state);

    static void encodeSoftClip(CodingState &state);

    void encodeSubstitution(CodingState &state);

    void encodeSplice(CodingState &state);

    void encodeMatch(CodingState &state);

    static bool updateCount(char cigar_char, CodingState &state);

    void encodeCigarToken(char cigar_char, CodingState &state);

    const core::record::alignment_split::SameRec &extractPairedAlignment(const core::record::Record &rec) const;

   public:
    explicit Encoder(uint64_t startingMappingPos);

    void add(const core::record::Record &rec, const std::string &ref1, const std::string &ref2);

    core::AccessUnitRaw &&moveStreams();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace basecoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------