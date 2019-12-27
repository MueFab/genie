/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "read-encoder.h"
#include <format/mpegg_p2/clutter.h>
#include <format/mpegg_rec/alignment-container.h>
#include <format/mpegg_rec/mpegg-record.h>
#include <format/mpegg_rec/segment.h>
#include <array>
#include "util/exceptions.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace lae {

// ---------------------------------------------------------------------------------------------------------------------

LocalAssemblyReadEncoder::LocalAssemblyReadEncoder()
    : container(util::make_unique<MpeggRawAu>(util::make_unique<format::mpegg_p2::ParameterSet>(), 0)),
      pos(0),
      readCounter(0) {}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyReadEncoder::encodeFirstSegment(const format::mpegg_rec::MpeggRecord *rec) {
    // TODO: Splices
    const auto ALIGNMENT_INDEX = 0;
    const auto ALIGNMENT = rec->getAlignment(ALIGNMENT_INDEX);  // TODO: Multiple alignments. Currently only 1 supported
    const auto RECORD_INDEX = 0;
    const auto RECORD = rec->getRecordSegment(RECORD_INDEX);  // First segment

    container->push(GenSub::RTYPE, uint8_t(rec->getClassID()));

    const auto POSITION = ALIGNMENT->getPosition() - pos;
    pos = ALIGNMENT->getPosition();
    container->push(GenSub::POS_MAPPING_FIRST, POSITION);

    const auto LENGTH = RECORD->getLength() - 1;
    container->push(GenSub::RLEN, LENGTH);

    const auto RCOMP = ALIGNMENT->getAlignment()->getRComp();
    container->push(GenSub::RCOMP, RCOMP);

    const auto FLAG_PCR = (rec->getFlags() & GenConst::FLAGS_PCR_DUPLICATE_MASK) >> GenConst::FLAGS_PCR_DUPLICATE_POS;
    container->push(GenSub::FLAGS_PCR_DUPLICATE, FLAG_PCR);
    const auto FLAG_QUAL = (rec->getFlags() & GenConst::FLAGS_QUALITY_FAIL_MASK) >> GenConst::FLAGS_QUALITY_FAIL_POS;
    container->push(GenSub::FLAGS_QUALITY_FAIL, FLAG_QUAL);
    const auto FLAG_PAIR = (rec->getFlags() & GenConst::FLAGS_PROPER_PAIR_MASK) >> GenConst::FLAGS_PROPER_PAIR_POS;
    container->push(GenSub::FLAGS_PROPER_PAIR, FLAG_PAIR);

    const auto MSCORE_INDEX = 0;
    const auto MSCORE = ALIGNMENT->getAlignment()->getMappingScore(MSCORE_INDEX);  // TODO: Multiple mapping scores
    container->push(GenSub::MSCORE, MSCORE);
}

// ---------------------------------------------------------------------------------------------------------------------

const format::mpegg_rec::SplitAlignmentSameRec *LocalAssemblyReadEncoder::extractPairedAlignment(
    const format::mpegg_rec::MpeggRecord *rec) const {
    const size_t SPLIT_INDEX = 0;      // TODO: More than 2 split alignments (even supported by standard?)
    const size_t ALIGNMENT_INDEX = 0;  // TODO: Multialignments
    const auto SAME_REC = format::mpegg_rec::SplitAlignment::SplitAlignmentType::SAME_REC;
    if (rec->getAlignment(ALIGNMENT_INDEX)->getSplitAlignment(SPLIT_INDEX)->getType() == SAME_REC) {
        const auto ALIGNMENT = rec->getAlignment(ALIGNMENT_INDEX)->getSplitAlignment(SPLIT_INDEX);
        return reinterpret_cast<const format::mpegg_rec::SplitAlignmentSameRec *>(ALIGNMENT);
    } else {
        UTILS_DIE("Only same record split alignments supported");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyReadEncoder::encodeAdditionalSegment(size_t length,
                                                       const format::mpegg_rec::SplitAlignmentSameRec *srec) {
    const auto LENGTH = length - 1;
    container->push(GenSub::RLEN, LENGTH);

    const auto RCOMP = srec->getAlignment()->getRComp();
    container->push(GenSub::RCOMP, RCOMP);

    const auto MSCORE_INDEX = 0;  // TODO: MSCORE depth != 1
    const auto MSCORE = srec->getAlignment()->getMappingScore(MSCORE_INDEX);
    container->push(GenSub::MSCORE, MSCORE);

    container->push(GenSub::PAIR_DECODING_CASE, GenConst::PAIR_SAME_RECORD);

    const uint32_t DELTA = srec->getDelta();
    const bool FIRST1 = srec->getDelta() >= 0;          // Alignment 1 was the first if delta is positive
    const auto SAME_REC_DATA = (DELTA << 1u) | FIRST1;  // FIRST1 is encoded in least significant bit
    container->push(GenSub::PAIR_SAME_REC, SAME_REC_DATA);
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyReadEncoder::add(const format::mpegg_rec::MpeggRecord *rec, const std::string &ref1,
                                   const std::string &ref2) {
    std::pair<ClipInformation, ClipInformation> clips;

    encodeFirstSegment(rec);

    const auto FIRST_RECORD_INDEX = 0;
    const auto SEQUENCE = *rec->getRecordSegment(FIRST_RECORD_INDEX)->getSequence();
    const auto ALIGNMENT_INDEX = 0;
    const auto CIGAR = *rec->getAlignment(ALIGNMENT_INDEX)->getAlignment()->getECigar();  // TODO: Multi-alignments
    encodeCigar(SEQUENCE, CIGAR, ref1, rec->getClassID(), &clips.first);

    // Check if record is paired
    if (rec->getNumberOfRecords() > 1) {
        const format::mpegg_rec::SplitAlignmentSameRec *srec = extractPairedAlignment(rec);
        const auto SECOND_RECORD_INDEX = 1;
        const auto LENGTH2 = rec->getRecordSegment(SECOND_RECORD_INDEX)->getLength();
        encodeAdditionalSegment(LENGTH2, srec);

        const auto SEQUENCE2 = *rec->getRecordSegment(SECOND_RECORD_INDEX)->getSequence();
        const auto CIGAR2 = *srec->getAlignment()->getECigar();
        encodeCigar(SEQUENCE2, CIGAR2, ref2, rec->getClassID(), &clips.second);
    }

    encodeClips(clips);
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyReadEncoder::codeInsertion(CodingState *state) {
    for (size_t i = 0; i < state->count; ++i) {
        container->push(GenSub::MMPOS_TERMINATOR, GenConst::MMPOS_PERSIST);

        const auto POSITION = state->read_pos - state->lastMisMatch;
        state->lastMisMatch = state->read_pos + 1;
        container->push(GenSub::MMPOS_POSITION, POSITION);

        container->push(GenSub::MMTYPE_TYPE, GenConst::MMTYPE_INSERTION);

        const auto SYMBOL = getAlphabetProperties(AlphabetID::ACGTN).inverseLut[state->read[state->read_pos]];
        state->read_pos++;
        container->push(GenSub::MMTYPE_INSERTION, SYMBOL);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyReadEncoder::codeDeletion(CodingState *state) {
    for (size_t i = 0; i < state->count; ++i) {
        container->push(GenSub::MMPOS_TERMINATOR, GenConst::MMPOS_PERSIST);

        const auto POSITION = state->read_pos - state->lastMisMatch;
        state->lastMisMatch = state->read_pos;
        container->push(GenSub::MMPOS_POSITION, POSITION);
        container->push(GenSub::MMPOS_POSITION, GenConst::MMTYPE_DELETION);
        state->ref_offset++;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyReadEncoder::codeHardClip(CodingState *state) {
    state->clips->hardClips[state->isRightClip] += state->count;
    state->ref_offset += state->count;
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyReadEncoder::codeSoftClip(CodingState *state) {
    for (size_t i = 0; i < state->count; ++i) {
        if (state->read_pos >= state->read.length()) {
            UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
        }
        state->clips->softClips[state->isRightClip] +=
            getAlphabetProperties(AlphabetID::ACGTN).inverseLut[state->read[state->read_pos]];
        state->read_pos++;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyReadEncoder::encodeSubstitution(CodingState *state) {
    if (state->ref[state->ref_offset] == 0) {
        const auto SYMBOL = getAlphabetProperties(AlphabetID::ACGTN).inverseLut[state->read[state->read_pos]];
        container->push(GenSub::UREADS, SYMBOL);
        return;
    }
    container->push(GenSub::MMPOS_TERMINATOR, GenConst::MMPOS_PERSIST);

    const auto POSITION = state->read_pos - state->lastMisMatch;
    state->lastMisMatch = state->read_pos + 1;
    container->push(GenSub::MMPOS_POSITION, POSITION);

    container->push(GenSub::MMTYPE_TYPE, GenConst::MMTYPE_SUBSTITUTION);
    if (state->type > format::mpegg_rec::MpeggRecord::ClassType::CLASS_N) {
        const auto SYMBOL = getAlphabetProperties(AlphabetID::ACGTN).inverseLut[state->read[state->read_pos]];
        container->push(GenSub::MMTYPE_SUBSTITUTION, SYMBOL);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyReadEncoder::encodeMatch(CodingState *state) {
    state->isRightClip = true;
    for (size_t i = 0; i < state->count; ++i) {
        if (state->read_pos >= state->read.length()) {
            UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
        }
        if (state->read[state->read_pos] != state->ref[state->ref_offset]) {
            encodeSubstitution(state);
        }

        state->read_pos++;
        state->ref_offset++;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool LocalAssemblyReadEncoder::updateCount(char cigar_char, CodingState *state) {
    if (cigar_char >= '0' && cigar_char <= '9') {
        state->count *= 10;
        state->count += cigar_char - '0';
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyReadEncoder::encodeCigarToken(char cigar_char, CodingState *state) {
    switch (cigar_char) {
        case 'M':
        case '=':
        case 'X':
            encodeMatch(state);
            break;

        case 'I':
            codeInsertion(state);
            break;
        case 'S':
        case 'P':
            codeSoftClip(state);
            break;

        case 'N':
        case 'D':
            codeDeletion(state);
            break;

        case 'H':
            codeHardClip(state);
            break;

        default:
            UTILS_THROW_RUNTIME_EXCEPTION("Unknown CIGAR character");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyReadEncoder::encodeCigar(const std::string &read, const std::string &cigar, const std::string &ref,
                                           format::mpegg_rec::MpeggRecord::ClassType type, ClipInformation *clips) {
    CodingState state(read, ref, type, clips);
    for (char cigar_char : cigar) {
        if (updateCount(cigar_char, &state)) {
            continue;
        }
        encodeCigarToken(cigar_char, &state);
        state.count = 0;
    }

    if (state.read_pos != read.length()) {
        UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
    }

    if (type > format::mpegg_rec::MpeggRecord::ClassType::CLASS_P) {
        container->push(GenSub::MMPOS_TERMINATOR, GenConst::MMPOS_TERMINATE);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<MpeggRawAu> LocalAssemblyReadEncoder::moveStreams() {
    auto tmp = std::move(container);
    container = util::make_unique<MpeggRawAu>(util::make_unique<format::mpegg_p2::ParameterSet>(), 0);
    return tmp;
}

// ---------------------------------------------------------------------------------------------------------------------

bool LocalAssemblyReadEncoder::encodeSingleClip(const ClipInformation &inf, bool last) {
    bool clips_present = false;
    for (const auto &hard : inf.hardClips) {
        clips_present = hard || clips_present;
    }
    for (const auto &soft : inf.softClips) {
        clips_present = clips_present || !soft.empty();
    }
    if (clips_present) {
        container->push(GenSub::CLIPS_RECORD_ID, readCounter);
    } else {
        return clips_present;
    }

    for (size_t index = 0; index < inf.softClips.size(); ++index) {
        if (inf.hardClips[index]) {
            const auto TYPE = 0x4u | (uint32_t(last) << 1u) | index;
            container->push(GenSub::CLIPS_TYPE, TYPE);
            container->push(GenSub::CLIPS_HARD_LENGTH, inf.hardClips[index]);
        } else if (!inf.softClips[index].empty()) {
            const auto TYPE = (uint32_t(last) << 1u) | index;
            container->push(GenSub::CLIPS_TYPE, TYPE);
            for (const auto &c : inf.softClips[index]) {
                container->push(GenSub::CLIPS_SOFT_STRING, c);
            }
            const auto TERMINATOR = getAlphabetProperties(AlphabetID::ACGTN).lut.size();  // TODO: other alphabets
            container->push(GenSub::CLIPS_SOFT_STRING, TERMINATOR);
        }
    }

    return clips_present;
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyReadEncoder::encodeClips(const std::pair<ClipInformation, ClipInformation> &clips) {
    bool present = encodeSingleClip(clips.first, false);
    present = encodeSingleClip(clips.second, true) || present;

    if (present) {
        container->push(GenSub::CLIPS_TYPE, GenConst::CLIPS_RECORD_END);
    }

    readCounter += 2;  // TODO: Check if this applies to unpaired situations
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace lae

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------