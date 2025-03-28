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
#include "genie/core/qv_decoder.h"
#include "genie/core/record/alignment_split/same-rec.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace basecoder {

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
        case core::gen_const::kPairSameRecord:
            std::get<1>(state).SetRead1First(meta.first1);
            break;
        case core::gen_const::kPairR1Split:
            std::get<1>(state).SetRead1First(false);
            std::get<0>(state).addAlignmentSplit(
                genie::util::make_unique<genie::core::record::alignment_split::OtherRec>(
                    container.pull(core::gen_sub::kPairR1Split), ref));
            break;
        case core::gen_const::kPairR2Split:
            std::get<1>(state).SetRead1First(true);
            std::get<0>(state).addAlignmentSplit(
                genie::util::make_unique<genie::core::record::alignment_split::OtherRec>(
                    container.pull(core::gen_sub::kPairR2Split), ref));
            break;
        case core::gen_const::kPairR1DiffRef:
            std::get<1>(state).SetRead1First(false);
            std::get<0>(state).addAlignmentSplit(
                genie::util::make_unique<genie::core::record::alignment_split::OtherRec>(
                    container.pull(core::gen_sub::kPairR1DiffPos),
                    (uint16_t)container.pull(core::gen_sub::kPairR1DiffSeq)));
            break;
        case core::gen_const::kPairR2DiffRef:
            std::get<1>(state).SetRead1First(true);
            std::get<0>(state).addAlignmentSplit(
                genie::util::make_unique<genie::core::record::alignment_split::OtherRec>(
                    container.pull(core::gen_sub::kPairR2DiffPos),
                    (uint16_t)container.pull(core::gen_sub::kPairR2DiffSeq)));
            break;
        case core::gen_const::kPairR1Unpaired:
            std::get<1>(state).SetRead1First(true);
            std::get<0>(state).addAlignmentSplit(
                genie::util::make_unique<genie::core::record::alignment_split::Unpaired>());
            break;
        case core::gen_const::kPairR2Unpaired :
            std::get<1>(state).SetRead1First(false);
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
    meta.position[0] = position + container.get(core::gen_sub::kPositionFirst).get(0);

    meta.num_segments = 1;
    if (number_template_segments == 2) {
        meta.decoding_case = uint8_t(container.pull(core::gen_sub::kPairDecodingCase));
        if (meta.decoding_case == core::gen_const::kPairSameRecord) {
            meta.num_segments = 2;
            const auto SAME_REC_DATA = (uint32_t)container.pull(core::gen_sub::kPairSameRec);
            meta.first1 = !(SAME_REC_DATA & 1u);
            const auto DELTA = (int16_t)(uint16_t)(SAME_REC_DATA >> 1u);
            meta.position[1] = meta.position[0] + DELTA;
        }
    }

    auto deletions = numberDeletions(meta.num_segments);
    for (size_t i = 0; i < meta.num_segments; ++i) {
        meta.length[i] = (length ? length : container.pull(core::gen_sub::kReadLength) + 1) + deletions[i];
    }
    return meta;
}

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<core::record::AlignmentBox, core::record::Record> Decoder::decode(size_t clip_offset, std::string &&seq,
                                                                             std::string &&cigar) {
    auto sequence = std::move(seq);

    const auto RTYPE = container.pull(core::gen_sub::kRtype);

    const auto RCOMP = (uint8_t)container.pull(core::gen_sub::kReverseComplement);

    const auto FLAG_PCR = container.isEnd(core::gen_sub::FlagsPcrDuplicate)
                              ? 0
                              : container.pull(core::gen_sub::FlagsPcrDuplicate)
                                    << core::gen_const::kFlagsPcrDuplicatePos;
    const auto FLAG_QUAL = container.isEnd(core::gen_sub::kFlagsQualityFail )
                               ? 0
                               : container.pull(core::gen_sub::kFlagsQualityFail )
                                     << core::gen_const::kFlagsQualityFailPos;
    const auto FLAG_PAIR = container.isEnd(core::gen_sub::kFlagsProperPair )
                               ? 0
                               : container.pull(core::gen_sub::kFlagsProperPair )
                                     << core::gen_const::kFlagsProperPairPos;
    const auto FLAGS = uint8_t(FLAG_PCR | FLAG_QUAL | FLAG_PAIR);

    const auto MSCORE = (uint32_t)container.pull(core::gen_sub::kMappingScore);

    // const auto RGROUP = container.isEnd(core::gen_sub::kReadGroup) ? 0 : container.pull(core::gen_sub::kReadGroup);

    position += container.pull(core::gen_sub::kPositionFirst);
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
            if (!GetAlphabetProperties(core::AlphabetId::kAcgtn).IsIncluded(cur_tok)) {
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
    if (!GetAlphabetProperties(core::AlphabetId::kAcgtn).IsIncluded(cur_tok)) {
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

    const auto RCOMP = (uint8_t)container.pull(core::gen_sub::kReverseComplement);
    const auto MSCORE = (int32_t)container.pull(core::gen_sub::kMappingScore);

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
    if (container.get(core::gen_sub::kMismatchPosTerminator).isEmpty() || container.get(core::gen_sub::kMismatchType).isEmpty()) {
        return counters;
    }
    size_t lookahead = 0;
    for (size_t r = 0; r < number; ++r) {
        while (!container.peek(core::gen_sub::kMismatchPosTerminator, lookahead + r)) {
            if (container.peek(core::gen_sub::kMismatchType, lookahead) == core::gen_const::kMismatchTypeDeletion) {
                counters[r]++;
            } else if (container.peek(core::gen_sub::kMismatchType, lookahead) == core::gen_const::kMismatchTypeInsertion) {
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
    if (container.isEnd(core::gen_sub::kMismatchPosTerminator)) {
        for (auto &c : sequence) {
            if (c == 0) {
                c = GetAlphabetProperties(core::AlphabetId::kAcgtn).lut[container.pull(core::gen_sub::kUnalignedReads)];
            }
        }
        return;
    }
    while (!container.pull(core::gen_sub::kMismatchPosTerminator)) {
        mismatchPosition += container.pull((core::gen_sub::kMismatchPosDelta)) + 1;
        const auto POSITION = mismatchPosition - 1 + clip_offset;
        const auto TYPE = container.get(core::gen_sub::kMismatchType).isEmpty()
                              ? core::gen_const::kMismatchTypeSubstitution
                              : container.pull((core::gen_sub::kMismatchType));
        if (TYPE == core::gen_const::kMismatchTypeSubstitution) {
            const auto SUBSTITUTION =
                container.get(core::gen_sub::kMismatchTypeSubstBase).getMismatchDecoder() &&
                        container.get(core::gen_sub::kMismatchTypeSubstBase).getMismatchDecoder()->dataLeft()
                    ? container.get(core::gen_sub::kMismatchTypeSubstBase)
                          .getMismatchDecoder()
                          ->decodeMismatch(
                              GetAlphabetProperties(core::AlphabetId::kAcgtn).inverseLut[sequence[POSITION]])
                    : GetAlphabetProperties(core::AlphabetId::kAcgtn).inverseLut['N'];
            const auto SUBSTITUTION_CHAR = GetAlphabetProperties(core::AlphabetId::kAcgtn).lut[SUBSTITUTION];
            sequence[POSITION] = SUBSTITUTION_CHAR;
            cigar_extended[POSITION + cigarOffset] = SUBSTITUTION_CHAR;
        } else if (TYPE == core::gen_const::kMismatchTypeInsertion) {
            const auto INSERTION = container.pull((core::gen_sub::kMismatchTypeInsert));
            const auto INSERTION_CHAR = GetAlphabetProperties(core::AlphabetId::kAcgtn).lut[INSERTION];
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
            c = GetAlphabetProperties(core::AlphabetId::kAcgtn).lut[container.pull(core::gen_sub::kUnalignedReads)];
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<size_t, size_t> Decoder::decodeClips(std::vector<std::string> &sequences,
                                                std::vector<std::string> &cigar_extended) {
    size_t num = recordCounter++;
    std::tuple<size_t, size_t> softclip_offset{0, 0};
    if (container.isEnd(core::gen_sub::kClipsRecordId) || num != container.peek(core::gen_sub::kClipsRecordId)) {
        return softclip_offset;
    }
    container.pull(core::gen_sub::kClipsRecordId);
    auto clipType = container.pull(core::gen_sub::kClipsType);
    while (clipType != core::gen_const::kClipsRecordEnd) {
        bool hardclip = clipType & 4u;
        size_t record_no = clipType & 2u ? sequences.size() - 1 : 0;
        bool end = clipType & 1u;

        if (hardclip) {
            const auto HARDCLIP_SIZE = container.pull(core::gen_sub::kClipsHardClip);
            size_t cigar_position = end ? cigar_extended[record_no].size() : 0;
            cigar_extended[record_no].insert(cigar_position, HARDCLIP_SIZE, ']');
        } else {
            std::string softClip;
            auto symbol = container.pull(core::gen_sub::kClipsSoftClip);
            while (symbol != GetAlphabetProperties(core::AlphabetId::kAcgtn).lut.size()) {
                softClip += GetAlphabetProperties(core::AlphabetId::kAcgtn).lut[symbol];
                symbol = container.pull(core::gen_sub::kClipsSoftClip);
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
        clipType = container.pull(core::gen_sub::kClipsType);
    }

    return softclip_offset;
}

// ---------------------------------------------------------------------------------------------------------------------

void Decoder::clear() { this->container.clear(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace basecoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
