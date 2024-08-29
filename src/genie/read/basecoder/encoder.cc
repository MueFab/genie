/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/read/basecoder/encoder.h"
#include <genie/core/record/alignment_split/other-rec.h>
#include <algorithm>
#include <array>
#include <iostream>
#include <utility>
#include "genie/core/parameter/parameter_set.h"
#include "genie/core/record/alignment-box.h"
#include "genie/core/record/alignment_split/same-rec.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::basecoder {

// ---------------------------------------------------------------------------------------------------------------------

Encoder::CodingState::CodingState(const std::string &_read, const std::string &_ref, core::record::ClassType _type)
    : count(0),
      read_pos(0),
      ref_offset(0),
      lastMisMatch(0),
      isRightClip(false),
      read(_read),
      ref(_ref),
      type(_type),
      clips() {}

// ---------------------------------------------------------------------------------------------------------------------

Encoder::Encoder(uint64_t startingMappingPos)
    : container(core::parameter::EncodingSet(), 0), pos(startingMappingPos), readCounter(0) {}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeFirstSegment(const core::record::Record &rec) {
    // TODO(Fabian): Splices
    const auto &ALIGNMENT =
        rec.getAlignments().front();            // TODO(Fabian): Multiple alignments. Currently only 1 supported
    const auto &RECORD = rec.getSegments()[0];  // First segment

    container.push(core::GenSub::RTYPE, uint8_t(rec.getClassID()));

    const auto POSITION = ALIGNMENT.getPosition() - pos;
    pos = ALIGNMENT.getPosition();
    container.push(core::GenSub::POS_MAPPING_FIRST, POSITION);

    const auto LENGTH = RECORD.getSequence().length() - 1;
    container.push(core::GenSub::RLEN, LENGTH);

    const auto RCOMP = ALIGNMENT.getAlignment().getRComp();
    container.push(core::GenSub::RCOMP, RCOMP);

    const auto FLAG_PCR =
        (rec.getFlags() & core::GenConst::FLAGS_PCR_DUPLICATE_MASK) >> core::GenConst::FLAGS_PCR_DUPLICATE_POS;
    container.push(core::GenSub::FLAGS_PCR_DUPLICATE, FLAG_PCR);
    const auto FLAG_QUAL =
        (rec.getFlags() & core::GenConst::FLAGS_QUALITY_FAIL_MASK) >> core::GenConst::FLAGS_QUALITY_FAIL_POS;
    container.push(core::GenSub::FLAGS_QUALITY_FAIL, FLAG_QUAL);
    const auto FLAG_PAIR =
        (rec.getFlags() & core::GenConst::FLAGS_PROPER_PAIR_MASK) >> core::GenConst::FLAGS_PROPER_PAIR_POS;
    container.push(core::GenSub::FLAGS_PROPER_PAIR, FLAG_PAIR);

    const auto MSCORE =
        ALIGNMENT.getAlignment().getMappingScores().empty()
            ? 255
            : ALIGNMENT.getAlignment().getMappingScores().front();  // TODO(Fabian): Multiple mapping scores
    container.push(core::GenSub::MSCORE, MSCORE);

    // const auto RGROUP = 0;  // TODO(Fabian)
    //  container.push(core::GenSub::RGROUP, RGROUP);
}

// ---------------------------------------------------------------------------------------------------------------------

const core::record::alignment_split::SameRec &Encoder::extractPairedAlignment(const core::record::Record &rec) const {
    // TODO(Fabian): More than 2 split alignments (even supported by standard?)
    // TODO(Fabian): Multialignments
    const auto SAME_REC = core::record::AlignmentSplit::Type::SAME_REC;
    UTILS_DIE_IF(rec.getAlignments().front().getAlignmentSplits().front()->getType() != SAME_REC,
                 "Only same record split alignments supported");
    const auto ALIGNMENT = rec.getAlignments().front().getAlignmentSplits().front().get();
    return *reinterpret_cast<const core::record::alignment_split::SameRec *>(ALIGNMENT);
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeAdditionalSegment(size_t length, const core::record::alignment_split::SameRec &srec, bool first1) {
    const auto LENGTH = length - 1;
    container.push(core::GenSub::RLEN, LENGTH);

    const auto RCOMP = srec.getAlignment().getRComp();
    container.push(core::GenSub::RCOMP, RCOMP);

    // TODO(Fabian): MSCORE depth != 1
    const auto MSCORE = srec.getAlignment().getMappingScores().front();
    container.push(core::GenSub::MSCORE, MSCORE);

    container.push(core::GenSub::PAIR_DECODING_CASE, core::GenConst::PAIR_SAME_RECORD);

    const auto DELTA = srec.getDelta();
    const auto SAME_REC_DATA = (DELTA << 1u) | uint32_t(!first1);  // FIRST1 is encoded in least significant bit
    container.push(core::GenSub::PAIR_SAME_REC, SAME_REC_DATA);
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::add(const core::record::Record &rec, const std::string &ref1, const std::string &ref2) {
    std::pair<ClipInformation, ClipInformation> clips;

    encodeFirstSegment(rec);

    const auto &SEQUENCE = rec.getSegments()[0].getSequence();
    const auto &CIGAR = rec.getAlignments().front().getAlignment().getECigar();  // TODO(Fabian): Multi-alignments
    clips.first = encodeCigar(SEQUENCE, CIGAR, ref1, rec.getClassID());

    // Check if record is paired
    if (rec.getSegments().size() > 1) {
        // Same record
        const core::record::alignment_split::SameRec &srec = extractPairedAlignment(rec);
        const auto &SEQUENCE2 = rec.getSegments()[1].getSequence();
        const auto LENGTH2 = SEQUENCE2.length();
        encodeAdditionalSegment(LENGTH2, srec, rec.isRead1First());

        const auto &CIGAR2 = srec.getAlignment().getECigar();
        clips.second = encodeCigar(SEQUENCE2, CIGAR2, ref2, rec.getClassID());
    } else if (rec.getNumberOfTemplateSegments() > 1) {
        // Unpaired
        if (rec.getAlignments().front().getAlignmentSplits().front()->getType() ==
            core::record::AlignmentSplit::Type::UNPAIRED) {
            if (rec.isRead1First()) {
                container.push(core::GenSub::PAIR_DECODING_CASE, core::GenConst::PAIR_R1_UNPAIRED);
            } else {
                container.push(core::GenSub::PAIR_DECODING_CASE, core::GenConst::PAIR_R2_UNPAIRED);
            }
            // Other record
        } else {
            const auto ALIGNMENT = rec.getAlignments().front().getAlignmentSplits().front().get();
            auto split = *reinterpret_cast<const core::record::alignment_split::OtherRec *>(ALIGNMENT);
            if (rec.isRead1First()) {
                if (split.getNextSeq() != rec.getAlignmentSharedData().getSeqID()) {
                    container.push(core::GenSub::PAIR_DECODING_CASE, core::GenConst::PAIR_R2_DIFF_REF);
                    container.push(core::GenSub::PAIR_R2_DIFF_SEQ, split.getNextSeq());
                    container.push(core::GenSub::PAIR_R2_DIFF_POS, split.getNextPos());
                } else {
                    container.push(core::GenSub::PAIR_DECODING_CASE, core::GenConst::PAIR_R2_SPLIT);
                    container.push(core::GenSub::PAIR_R2_SPLIT, split.getNextPos());
                }
            } else {
                if (split.getNextSeq() != rec.getAlignmentSharedData().getSeqID()) {
                    container.push(core::GenSub::PAIR_DECODING_CASE, core::GenConst::PAIR_R1_DIFF_REF);
                    container.push(core::GenSub::PAIR_R1_DIFF_SEQ, split.getNextSeq());
                    container.push(core::GenSub::PAIR_R1_DIFF_POS, split.getNextPos());
                } else {
                    container.push(core::GenSub::PAIR_DECODING_CASE, core::GenConst::PAIR_R1_SPLIT);
                    container.push(core::GenSub::PAIR_R1_SPLIT, split.getNextPos());
                }
            }
        }
    }

    encodeClips(clips);

    container.addRecord();
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeInsertion(CodingState &state) {
    for (size_t i = 0; i < state.count; ++i) {
        container.push(core::GenSub::MMPOS_TERMINATOR, core::GenConst::MMPOS_PERSIST);

        const auto POSITION = state.read_pos - state.lastMisMatch - state.clips.softClips[0].length();
        state.lastMisMatch = state.read_pos + 1 - state.clips.softClips[0].length();
        container.push(core::GenSub::MMPOS_POSITION, POSITION);

        container.push(core::GenSub::MMTYPE_TYPE, core::GenConst::MMTYPE_INSERTION);

        const auto SYMBOL = getAlphabetProperties(core::AlphabetID::ACGTN).inverseLut[state.read[state.read_pos]];
        state.read_pos++;
        container.push(core::GenSub::MMTYPE_INSERTION, SYMBOL);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeDeletion(CodingState &state) {
    for (size_t i = 0; i < state.count; ++i) {
        container.push(core::GenSub::MMPOS_TERMINATOR, core::GenConst::MMPOS_PERSIST);

        const auto POSITION = state.read_pos - state.lastMisMatch - state.clips.softClips[0].length();
        state.lastMisMatch = state.read_pos - state.clips.softClips[0].length();
        container.push(core::GenSub::MMPOS_POSITION, POSITION);
        container.push(core::GenSub::MMTYPE_TYPE, core::GenConst::MMTYPE_DELETION);
        state.ref_offset++;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeHardClip(CodingState &state) { state.clips.hardClips[state.isRightClip] += state.count; }

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeSoftClip(CodingState &state) {
    for (size_t i = 0; i < state.count; ++i) {
        if (state.read_pos >= state.read.length()) {
            UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
        }
        state.clips.softClips[state.isRightClip] +=
            getAlphabetProperties(core::AlphabetID::ACGTN).inverseLut[state.read[state.read_pos]];
        state.read_pos++;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeSubstitution(CodingState &state) {
    if (state.ref[state.ref_offset] == 0) {
        const auto SYMBOL = getAlphabetProperties(core::AlphabetID::ACGTN).inverseLut[state.read[state.read_pos]];
        container.push(core::GenSub::UREADS, SYMBOL);
        return;
    }
    container.push(core::GenSub::MMPOS_TERMINATOR, core::GenConst::MMPOS_PERSIST);

    const auto POSITION = state.read_pos - state.lastMisMatch - state.clips.softClips[0].length();
    state.lastMisMatch = state.read_pos + 1 - state.clips.softClips[0].length();
    container.push(core::GenSub::MMPOS_POSITION, POSITION);

    if (state.type >= core::record::ClassType::CLASS_M) {
        container.push(core::GenSub::MMTYPE_TYPE, core::GenConst::MMTYPE_SUBSTITUTION);
        const auto SYMBOL = getAlphabetProperties(core::AlphabetID::ACGTN).inverseLut[state.read[state.read_pos]];
        container.push(core::GenSub::MMTYPE_SUBSTITUTION, SYMBOL);
        container.pushDependency(
            core::GenSub::MMTYPE_SUBSTITUTION,
            getAlphabetProperties(core::AlphabetID::ACGTN).inverseLut[state.ref[state.ref_offset]]);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeMatch(CodingState &state) {
    state.isRightClip = true;
    for (size_t i = 0; i < state.count; ++i) {
        if (state.read_pos >= state.read.length()) {
            UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
        }
        if (state.read[state.read_pos] != state.ref[state.ref_offset]) {
            encodeSubstitution(state);
        }

        state.read_pos++;
        state.ref_offset++;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool Encoder::updateCount(char cigar_char, CodingState &state) {
    if (isdigit(cigar_char)) {
        state.count *= 10;
        state.count += cigar_char - '0';
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeCigarToken(char cigar_char, CodingState &state) {
    if (getAlphabetProperties(core::AlphabetID::ACGTN).isIncluded(cigar_char)) {  // TODO(Fabian): other alphabets
        const char AMBIGUOUS_CHAR = '-';  // Character used twice in ecigar (deletion + alphabet 2)
        if ((cigar_char == AMBIGUOUS_CHAR && state.count == 0) || cigar_char != AMBIGUOUS_CHAR) {
            state.count = std::max(state.count, size_t(1));
            encodeMatch(state);
            return;
        }
    }
    switch (cigar_char) {
        case '=':
            encodeMatch(state);
            break;
        case '+':
            encodeInsertion(state);
            break;
        case '-':
            encodeDeletion(state);
            break;
        case ')':
            encodeSoftClip(state);
            break;
        case ']':
            encodeHardClip(state);
            break;
        case '%':
        case '/':
        case '*':
            encodeSplice(state);
            break;
        case '(':
        case '[':
            break;  // Ignore opening braces, so that number can be parsed. Closing braces will be handled
        default:
            UTILS_THROW_RUNTIME_EXCEPTION("Unknown CIGAR character");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Encoder::ClipInformation Encoder::encodeCigar(const std::string &read, const std::string &cigar, const std::string &ref,
                                              core::record::ClassType type) {
    CodingState state(read, ref, type);
    for (char cigar_char : cigar) {
        if (updateCount(cigar_char, state)) {
            continue;
        }
        encodeCigarToken(cigar_char, state);
        state.count = 0;
    }

    if (state.read_pos != read.length()) {
        UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
    }

    if (type > core::record::ClassType::CLASS_P) {
        container.push(core::GenSub::MMPOS_TERMINATOR, core::GenConst::MMPOS_TERMINATE);
    }
    return state.clips;
}

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit &&Encoder::moveStreams() { return std::move(container); }

// ---------------------------------------------------------------------------------------------------------------------

bool Encoder::encodeSingleClip(const ClipInformation &inf, bool last) {
    bool clips_present = false;
    for (const auto &hard : inf.hardClips) {
        clips_present = hard || clips_present;
    }
    for (const auto &soft : inf.softClips) {
        clips_present = clips_present || !soft.empty();
    }
    if (!clips_present) {
        return clips_present;
    }

    for (size_t index = 0; index < inf.softClips.size(); ++index) {
        if (!inf.softClips[index].empty()) {
            const auto TYPE = (uint32_t(last) << 1u) | index;
            container.push(core::GenSub::CLIPS_TYPE, TYPE);
            for (const auto &c : inf.softClips[index]) {
                container.push(core::GenSub::CLIPS_SOFT_STRING, c);
            }
            const auto TERMINATOR =
                getAlphabetProperties(core::AlphabetID::ACGTN).lut.size();  // TODO(Fabian): other alphabets
            container.push(core::GenSub::CLIPS_SOFT_STRING, TERMINATOR);
        } else if (inf.hardClips[index]) {
            const auto TYPE = 0x4u | (uint32_t(last) << 1u) | index;
            container.push(core::GenSub::CLIPS_TYPE, TYPE);
            container.push(core::GenSub::CLIPS_HARD_LENGTH, inf.hardClips[index]);
        }
    }

    return clips_present;
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeClips(const std::pair<ClipInformation, ClipInformation> &clips) {
    bool present = encodeSingleClip(clips.first, false);
    present = encodeSingleClip(clips.second, true) || present;

    if (present) {
        container.push(core::GenSub::CLIPS_RECORD_ID, readCounter);
        container.push(core::GenSub::CLIPS_TYPE, core::GenConst::CLIPS_RECORD_END);
    }

    readCounter += 1;  // TODO(Fabian): Check if this applies to unpaired situations
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::encodeSplice(Encoder::CodingState &state) {
    (void)state;
    UTILS_DIE("Splicing is currently not supported");
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::basecoder

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
