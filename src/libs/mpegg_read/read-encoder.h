/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_READ_ENCODER_H
#define GENIE_READ_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <backbone/mpegg_rec/split_alignment/split-alignment-same-rec.h>
#include <memory>
#include <string>
#include <vector>
#include "backbone/mpegg-raw-au.h"
#include "backbone/mpegg_rec/mpegg-record.h"

// ---------------------------------------------------------------------------------------------------------------------
namespace genie {
namespace mpegg_read {

// ---------------------------------------------------------------------------------------------------------------------

/**
 *
 */
class LocalAssemblyReadEncoder {
   private:
    genie::MpeggRawAu container;
    int32_t pos;
    uint32_t readCounter;

    void encodeFirstSegment(const genie::mpegg_rec::MpeggRecord &rec);

    void encodeAdditionalSegment(size_t length, const genie::mpegg_rec::SplitAlignmentSameRec &srec);

    struct ClipInformation {
        std::array<std::string, 2> softClips;
        std::array<size_t, 2> hardClips = {0, 0};
    };

    ClipInformation encodeCigar(const std::string &read, const std::string &cigar, const std::string &ref,
                                genie::mpegg_rec::ClassType type);

    bool encodeSingleClip(const ClipInformation &inf, bool last);

    void encodeClips(const std::pair<ClipInformation, ClipInformation> &clips);

    struct CodingState {
        CodingState(const std::string &_read, const std::string &_ref, genie::mpegg_rec::ClassType _type)
            : count(0),
              read_pos(0),
              ref_offset(0),
              lastMisMatch(0),
              isRightClip(false),
              read(_read),
              ref(_ref),
              type(_type),
              clips() {}
        size_t count;
        size_t read_pos;
        size_t ref_offset;
        size_t lastMisMatch;
        bool isRightClip;

        const std::string &read;
        const std::string &ref;
        const genie::mpegg_rec::ClassType type;
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

    const genie::mpegg_rec::SplitAlignmentSameRec &extractPairedAlignment(
        const genie::mpegg_rec::MpeggRecord &rec) const;

   public:
    explicit LocalAssemblyReadEncoder();

    void add(const genie::mpegg_rec::MpeggRecord &rec, const std::string &ref1, const std::string &ref2);

    genie::MpeggRawAu &&moveStreams();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_READ_ENCODER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------