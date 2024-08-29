/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/read/basecoder/decoder.h"
#include <genie/core/record/alignment_split/other-rec.h>
#include <genie/core/record/alignment_split/unpaired.h>
#include <tuple>
#include <utility>
#include "genie/core/qv-decoder.h"
#include "genie/core/record/alignment_split/same-rec.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::basecoder {

// ---------------------------------------------------------------------------------------------------------------------

Decoder::Decoder(core::AccessUnit &&au, size_t segments, size_t pos)
    : container(std::move(au)), position(pos), length(0), recordCounter(0), number_template_segments(segments) {}

// ---------------------------------------------------------------------------------------------------------------------

core::record::Record Decoder::pull(uint16_t ref, std::vector<std::string> &&vec, const SegmentMeta &meta) {
    std::vector<std::string> sequences = std::move(vec);
    std::vector<std::string> cigars;
    cigars.reserve(sequences.size());
    for (auto &sequence : sequences) {
        cigars.emplace_back(sequence.size(), '=');
    }
    auto clip_offset = decodeClips(sequences, cigars);

    auto state = decode(std::get<0>(clip_offset), std::move(sequences.front()), std::move(cigars.front()));
    switch (meta.decoding_case) {
        case core::GenConst::PAIR_SAME_RECORD:
            std::get<1>(state).setRead1First(meta.first1);
            break;
        case core::GenConst::PAIR_R1_SPLIT:
            std::get<1>(state).setRead1First(false);
            std::get<0>(state).addAlignmentSplit(
                genie::util::make_unique<genie::core::record::alignment_split::OtherRec>(
                    container.pull(core::GenSub::PAIR_R1_SPLIT), ref));
            break;
        case core::GenConst::PAIR_R2_SPLIT:
            std::get<1>(state).setRead1First(true);
            std::get<0>(state).addAlignmentSplit(
                genie::util::make_unique<genie::core::record::alignment_split::OtherRec>(
                    container.pull(core::GenSub::PAIR_R2_SPLIT), ref));
            break;
        case core::GenConst::PAIR_R1_DIFF_REF:
            std::get<1>(state).setRead1First(false);
            std::get<0>(state).addAlignmentSplit(
                genie::util::make_unique<genie::core::record::alignment_split::OtherRec>(
                    container.pull(core::GenSub::PAIR_R1_DIFF_POS),
                    (uint16_t)container.pull(core::GenSub::PAIR_R1_DIFF_SEQ)));
            break;
        case core::GenConst::PAIR_R2_DIFF_REF:
            std::get<1>(state).setRead1First(true);
            std::get<0>(state).addAlignmentSplit(
                genie::util::make_unique<genie::core::record::alignment_split::OtherRec>(
                    container.pull(core::GenSub::PAIR_R2_DIFF_POS),
                    (uint16_t)container.pull(core::GenSub::PAIR_R2_DIFF_SEQ)));
            break;
        case core::GenConst::PAIR_R1_UNPAIRED:
            std::get<1>(state).setRead1First(true);
            std::get<0>(state).addAlignmentSplit(
                genie::util::make_unique<genie::core::record::alignment_split::Unpaired>());
            break;
        case core::GenConst::PAIR_R2_UNPAIRED:
            std::get<1>(state).setRead1First(false);
            std::get<0>(state).addAlignmentSplit(
                genie::util::make_unique<genie::core::record::alignment_split::Unpaired>());
            break;
    }
    for (size_t i = 1; i < sequences.size(); ++i) {
        decodeAdditional(std::get<1>(clip_offset), std::move(sequences[i]), std::move(cigars[i]),
                         uint16_t(meta.position[1] - meta.position[0]), state);
    }

    std::get<1>(state).addAlignment(ref, std::move(std::get<0>(state)));
    return std::get<1>(state);
}

// ---------------------------------------------------------------------------------------------------------------------

Decoder::SegmentMeta Decoder::readSegmentMeta() {
    SegmentMeta meta{};
    meta.position[0] = position + container.get(core::GenSub::POS_MAPPING_FIRST).get(0);

    meta.num_segments = 1;
    if (number_template_segments == 2) {
        meta.decoding_case = uint8_t(container.pull(core::GenSub::PAIR_DECODING_CASE));
        if (meta.decoding_case == core::GenConst::PAIR_SAME_RECORD) {
            meta.num_segments = 2;
            const auto SAME_REC_DATA = (uint32_t)container.pull(core::GenSub::PAIR_SAME_REC);
            meta.first1 = !(SAME_REC_DATA & 1u);
            const auto DELTA = (int16_t)(uint16_t)(SAME_REC_DATA >> 1u);
            meta.position[1] = meta.position[0] + DELTA;
        }
    }

    auto deletions = numberDeletions(meta.num_segments);
    for (size_t i = 0; i < meta.num_segments; ++i) {
        meta.length[i] = (length ? length : container.pull(core::GenSub::RLEN) + 1) + deletions[i];
    }
    return meta;
}

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<core::record::AlignmentBox, core::record::Record> Decoder::decode(size_t clip_offset, std::string &&seq,
                                                                             std::string &&cigar) {
    auto sequence = std::move(seq);

    const auto RTYPE = container.pull(core::GenSub::RTYPE);

    const auto RCOMP = (uint8_t)container.pull(core::GenSub::RCOMP);

    const auto FLAG_PCR = container.isEnd(core::GenSub::FLAGS_PCR_DUPLICATE)
                              ? 0
                              : container.pull(core::GenSub::FLAGS_PCR_DUPLICATE)
                                    << core::GenConst::FLAGS_PCR_DUPLICATE_POS;
    const auto FLAG_QUAL = container.isEnd(core::GenSub::FLAGS_QUALITY_FAIL)
                               ? 0
                               : container.pull(core::GenSub::FLAGS_QUALITY_FAIL)
                                     << core::GenConst::FLAGS_QUALITY_FAIL_POS;
    const auto FLAG_PAIR = container.isEnd(core::GenSub::FLAGS_PROPER_PAIR)
                               ? 0
                               : container.pull(core::GenSub::FLAGS_PROPER_PAIR)
                                     << core::GenConst::FLAGS_PROPER_PAIR_POS;
    const auto FLAGS = uint8_t(FLAG_PCR | FLAG_QUAL | FLAG_PAIR);

    const auto MSCORE = (uint32_t)container.pull(core::GenSub::MSCORE);

    // const auto RGROUP = container.isEnd(core::GenSub::RGROUP) ? 0 : container.pull(core::GenSub::RGROUP);

    position += container.pull(core::GenSub::POS_MAPPING_FIRST);
    const auto POSITION = position;

    std::string ecigar = std::move(cigar);
    decodeMismatches(clip_offset, sequence, ecigar);

    core::record::Alignment alignment(contractECigar(ecigar), RCOMP);
    alignment.addMappingScore((int32_t)MSCORE);

    std::tuple<core::record::AlignmentBox, core::record::Record> ret;
    std::get<0>(ret) = core::record::AlignmentBox(POSITION, std::move(alignment));

    std::get<1>(ret) =
        core::record::Record((uint8_t)number_template_segments, core::record::ClassType(RTYPE), "", "", FLAGS);

    core::record::Segment segment(std::move(sequence));
    std::get<1>(ret).addSegment(std::move(segment));
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string Decoder::contractECigar(const std::string &cigar_long) {
    char cur_tok = cigar_long.front();
    size_t count = 0;
    std::string result;
    for (const auto &c : cigar_long) {
        if (c != cur_tok) {
            if (cur_tok == ']') {
                result += '[';
            }
            if (cur_tok == ')') {
                result += '(';
            }
            if (!getAlphabetProperties(core::AlphabetID::ACGTN).isIncluded(cur_tok)) {
                result += std::to_string(count);
                result += cur_tok;
            } else {
                for (size_t i = 0; i < count; ++i) {
                    result += cur_tok;
                }
            }
            cur_tok = c;
            count = 0;
        }
        count++;
    }
    if (cur_tok == ']') {
        result += '[';
    }
    if (cur_tok == ')') {
        result += '(';
    }
    if (!getAlphabetProperties(core::AlphabetID::ACGTN).isIncluded(cur_tok)) {
        result += std::to_string(count);
        result += cur_tok;
    } else {
        for (size_t i = 0; i < count; ++i) {
            result += cur_tok;
        }
    }
    return result;
}

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::decodeAdditional(size_t softclip_offset, std::string &&seq, std::string &&cigar, uint16_t delta_pos,
                               std::tuple<core::record::AlignmentBox, core::record::Record> &state) {
    auto sequence = std::move(seq);

    const auto RCOMP = (uint8_t)container.pull(core::GenSub::RCOMP);
    const auto MSCORE = (int32_t)container.pull(core::GenSub::MSCORE);

    std::string ecigar = std::move(cigar);
    decodeMismatches(softclip_offset, sequence, ecigar);

    core::record::Alignment alignment(contractECigar(ecigar), RCOMP);
    alignment.addMappingScore(MSCORE);
    std::get<0>(state).addAlignmentSplit(
        genie::util::make_unique<genie::core::record::alignment_split::SameRec>(delta_pos, alignment));

    core::record::Segment segment(std::move(sequence));
    std::get<1>(state).addSegment(std::move(segment));
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<int32_t> Decoder::numberDeletions(size_t number) {
    std::vector<int32_t> counters(number, 0);
    if (container.get(core::GenSub::MMPOS_TERMINATOR).isEmpty() || container.get(core::GenSub::MMTYPE_TYPE).isEmpty()) {
        return counters;
    }
    size_t lookahead = 0;
    for (size_t r = 0; r < number; ++r) {
        while (!container.peek(core::GenSub::MMPOS_TERMINATOR, lookahead + r)) {
            if (container.peek(core::GenSub::MMTYPE_TYPE, lookahead) == core::GenConst::MMTYPE_DELETION) {
                counters[r]++;
            } else if (container.peek(core::GenSub::MMTYPE_TYPE, lookahead) == core::GenConst::MMTYPE_INSERTION) {
                counters[r]--;
            }
            lookahead++;
        }
    }
    return counters;
}

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::decodeMismatches(size_t clip_offset, std::string &sequence, std::string &cigar_extended) {
    uint64_t mismatchPosition = 0;
    auto startPos = cigar_extended.find_first_not_of(']');
    uint64_t cigarOffset = startPos == std::string::npos ? 0 : startPos;
    if (container.isEnd(core::GenSub::MMPOS_TERMINATOR)) {
        for (auto &c : sequence) {
            if (c == 0) {
                c = getAlphabetProperties(core::AlphabetID::ACGTN).lut[container.pull(core::GenSub::UREADS)];
            }
        }
        return;
    }
    while (!container.pull(core::GenSub::MMPOS_TERMINATOR)) {
        mismatchPosition += container.pull((core::GenSub::MMPOS_POSITION)) + 1;
        const auto POSITION = mismatchPosition - 1 + clip_offset;
        const auto TYPE = container.get(core::GenSub::MMTYPE_TYPE).isEmpty()
                              ? core::GenConst::MMTYPE_SUBSTITUTION
                              : container.pull((core::GenSub::MMTYPE_TYPE));
        if (TYPE == core::GenConst::MMTYPE_SUBSTITUTION) {
            const auto SUBSTITUTION =
                container.get(core::GenSub::MMTYPE_SUBSTITUTION).getMismatchDecoder() &&
                        container.get(core::GenSub::MMTYPE_SUBSTITUTION).getMismatchDecoder()->dataLeft()
                    ? container.get(core::GenSub::MMTYPE_SUBSTITUTION)
                          .getMismatchDecoder()
                          ->decodeMismatch(
                              getAlphabetProperties(core::AlphabetID::ACGTN).inverseLut[sequence[POSITION]])
                    : getAlphabetProperties(core::AlphabetID::ACGTN).inverseLut['N'];
            const auto SUBSTITUTION_CHAR = getAlphabetProperties(core::AlphabetID::ACGTN).lut[SUBSTITUTION];
            sequence[POSITION] = SUBSTITUTION_CHAR;
            cigar_extended[POSITION + cigarOffset] = SUBSTITUTION_CHAR;
        } else if (TYPE == core::GenConst::MMTYPE_INSERTION) {
            const auto INSERTION = container.pull((core::GenSub::MMTYPE_INSERTION));
            const auto INSERTION_CHAR = getAlphabetProperties(core::AlphabetID::ACGTN).lut[INSERTION];
            sequence.insert(POSITION, 1, INSERTION_CHAR);
            cigar_extended.insert(POSITION + cigarOffset, 1, '+');
        } else {
            sequence.erase(POSITION, 1);
            cigar_extended[POSITION + cigarOffset] = '-';
            cigarOffset += 1;
            mismatchPosition -= 1;
        }
    }

    for (auto &c : sequence) {
        if (c == 0) {
            c = getAlphabetProperties(core::AlphabetID::ACGTN).lut[container.pull(core::GenSub::UREADS)];
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<size_t, size_t> Decoder::decodeClips(std::vector<std::string> &sequences,
                                                std::vector<std::string> &cigar_extended) {
    size_t num = recordCounter++;
    std::tuple<size_t, size_t> softclip_offset{0, 0};
    if (container.isEnd(core::GenSub::CLIPS_RECORD_ID) || num != container.peek(core::GenSub::CLIPS_RECORD_ID)) {
        return softclip_offset;
    }
    container.pull(core::GenSub::CLIPS_RECORD_ID);
    auto clipType = container.pull(core::GenSub::CLIPS_TYPE);
    while (clipType != core::GenConst::CLIPS_RECORD_END) {
        bool hardclip = clipType & 4u;
        size_t record_no = clipType & 2u ? sequences.size() - 1 : 0;
        bool end = clipType & 1u;

        if (hardclip) {
            const auto HARDCLIP_SIZE = container.pull(core::GenSub::CLIPS_HARD_LENGTH);
            size_t cigar_position = end ? cigar_extended[record_no].size() : 0;
            cigar_extended[record_no].insert(cigar_position, HARDCLIP_SIZE, ']');
        } else {
            std::string softClip;
            auto symbol = container.pull(core::GenSub::CLIPS_SOFT_STRING);
            while (symbol != getAlphabetProperties(core::AlphabetID::ACGTN).lut.size()) {
                softClip += getAlphabetProperties(core::AlphabetID::ACGTN).lut[symbol];
                symbol = container.pull(core::GenSub::CLIPS_SOFT_STRING);
            }

            sequences[record_no].erase(sequences[record_no].length() - softClip.length());
            size_t cigar_position = end ? cigar_extended[record_no].find_last_not_of(']') + 1 - softClip.size()
                                        : cigar_extended[record_no].find_first_not_of(']');
            size_t seq_position = end ? sequences[record_no].length() : 0;
            sequences[record_no].insert(seq_position, softClip);
            for (size_t i = 0; i < softClip.size(); ++i) {
                cigar_extended[record_no][cigar_position++] = ')';
            }

            if (seq_position == 0) {
                if (record_no == 0) {
                    std::get<0>(softclip_offset) = softClip.length();  // TODO(Fabian): not working for paired
                } else {
                    std::get<1>(softclip_offset) = softClip.length();
                }
            }
        }
        clipType = container.pull(core::GenSub::CLIPS_TYPE);
    }

    return softclip_offset;
}

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::clear() { this->container.clear(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::basecoder

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
