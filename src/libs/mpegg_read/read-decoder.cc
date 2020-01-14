#include "read-decoder.h"
namespace genie {
namespace mpegg_read {
    LocalAssemblyReadDecoder::LocalAssemblyReadDecoder(genie::MpeggRawAu &&au, size_t segments)
        : container(std::move(au)), position(0), length(0), recordCounter(0), number_template_segments(segments) {}
    genie::mpegg_rec::MpeggRecord LocalAssemblyReadDecoder::pull(uint16_t ref, std::vector<std::string> &&vec) {
        std::vector<std::string> sequences = std::move(vec);
        std::vector<std::string> cigars;
        cigars.reserve(sequences.size());
        for (auto &sequence : sequences) {
            cigars.emplace_back(sequence.size(), '=');
        }
        auto clip_offset = decodeClips(sequences, cigars);
        auto state = decode(std::get<0>(clip_offset), std::move(sequences.front()), std::move(cigars.front()));
        for (size_t i = 1; i < sequences.size(); ++i) {
            decodeAdditional(std::get<1>(clip_offset), std::move(sequences[i]), std::move(cigars[i]), state);
        }

        std::get<1>(state).addAlignment(ref, std::move(std::get<0>(state)));
        return std::get<1>(state);
    }

    std::vector<LocalAssemblyReadDecoder::SegmentMeta> LocalAssemblyReadDecoder::readSegmentMeta() {
        std::vector<SegmentMeta> metaData;
        uint64_t offset = 0;
        auto deletions = numberDeletions(number_template_segments);
        for (size_t i = 0; i < number_template_segments; ++i) {
            const auto LENGTH = (length ? length : container.pull(genie::GenSub::RLEN) + 1) + deletions[i];
            offset += container.get(genie::GenSub::POS_MAPPING_FIRST).get(i);
            const auto POSITION = position + offset;
            metaData.emplace_back(SegmentMeta{POSITION, LENGTH});
        }
        return metaData;
    }

    std::tuple<genie::mpegg_rec::AlignmentContainer, genie::mpegg_rec::MpeggRecord> LocalAssemblyReadDecoder::decode(size_t clip_offset,
                                                                                           std::string &&seq,
                                                                                           std::string &&cigar) {
        auto sequence = std::move(seq);

        const auto RTYPE = container.pull(genie::GenSub::RTYPE);

        const auto RCOMP = container.pull(genie::GenSub::RCOMP);

        const auto FLAG_PCR = container.pull(genie::GenSub::FLAGS_PCR_DUPLICATE)
            << genie::GenConst::FLAGS_PCR_DUPLICATE_POS;
        const auto FLAG_QUAL = container.pull(genie::GenSub::FLAGS_QUALITY_FAIL)
            << genie::GenConst::FLAGS_QUALITY_FAIL_POS;
        const auto FLAG_PAIR = container.pull(genie::GenSub::FLAGS_PROPER_PAIR)
            << genie::GenConst::FLAGS_PROPER_PAIR_POS;
        const auto FLAGS = FLAG_PCR | FLAG_QUAL | FLAG_PAIR;

        const auto MSCORE = container.pull(genie::GenSub::MSCORE);

        const auto RGROUP = container.pull(genie::GenSub::RGROUP);

        position += container.pull(genie::GenSub::POS_MAPPING_FIRST);
        const auto POSITION = position;

        std::string ecigar = std::move(cigar);
        decodeMismatches(clip_offset, sequence, ecigar);

        genie::mpegg_rec::Alignment alignment(contractECigar(ecigar), RCOMP);
        alignment.addMappingScore(MSCORE);

        std::tuple<genie::mpegg_rec::AlignmentContainer, genie::mpegg_rec::MpeggRecord> ret;
        std::get<0>(ret) = genie::mpegg_rec::AlignmentContainer(POSITION, std::move(alignment));

        std::get<1>(ret) = genie::mpegg_rec::MpeggRecord(number_template_segments, genie::mpegg_rec::ClassType(RTYPE),
                                                         "", std::to_string(RGROUP), FLAGS);

        genie::mpegg_rec::Segment segment(std::move(sequence));
        std::get<1>(ret).addRecordSegment(std::move(segment));
        return ret;
    }

    std::string LocalAssemblyReadDecoder::contractECigar(const std::string &cigar_long) {
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
                if (!getAlphabetProperties(genie::AlphabetID::ACGTN).isIncluded(cur_tok)) {
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
        if (!getAlphabetProperties(genie::AlphabetID::ACGTN).isIncluded(cur_tok)) {
            result += std::to_string(count);
            result += cur_tok;
        } else {
            for (size_t i = 0; i < count; ++i) {
                result += cur_tok;
            }
        }
        return result;
    }

    void LocalAssemblyReadDecoder::decodeAdditional(size_t softclip_offset, std::string &&seq, std::string &&cigar,
                          std::tuple<genie::mpegg_rec::AlignmentContainer, genie::mpegg_rec::MpeggRecord> &state) {
        auto sequence = std::move(seq);

        const auto RCOMP = container.pull(genie::GenSub::RCOMP);

        const auto MSCORE = container.pull(genie::GenSub::MSCORE);

        std::string ecigar = std::move(cigar);
        decodeMismatches(softclip_offset, sequence, ecigar);

        const auto PAIRING_CASE = container.pull(genie::GenSub::PAIR_DECODING_CASE);
        if (PAIRING_CASE != genie::GenConst::PAIR_SAME_RECORD) {
            UTILS_DIE("Same-Record pair decoding supported only!");
        }

        const auto SAME_REC_DATA = container.pull(genie::GenSub::PAIR_SAME_REC);
        //        const bool FIRST1 = SAME_REC_DATA & 1u;
        const uint32_t DELTA = SAME_REC_DATA >> 1u;

        genie::mpegg_rec::Alignment alignment(contractECigar(ecigar), RCOMP);
        alignment.addMappingScore(MSCORE);
        auto srec = util::make_unique<genie::mpegg_rec::SplitAlignmentSameRec>(DELTA, std::move(alignment));
        std::get<0>(state).addSplitAlignment(std::move(srec));

        genie::mpegg_rec::Segment segment(std::move(sequence));
        std::get<1>(state).addRecordSegment(std::move(segment));
    }

    std::vector<int32_t> LocalAssemblyReadDecoder::numberDeletions(size_t number) {
        std::vector<int32_t> counters(number, 0);
        size_t lookahead = 0;
        for (size_t r = 0; r < number; ++r) {
            while (!container.peek(genie::GenSub::MMPOS_TERMINATOR, lookahead + r)) {
                if (container.peek(genie::GenSub::MMTYPE_TYPE, lookahead) == genie::GenConst::MMTYPE_DELETION) {
                    counters[r]++;
                } else if (container.peek(genie::GenSub::MMTYPE_TYPE, lookahead) == genie::GenConst::MMTYPE_INSERTION) {
                    counters[r]--;
                }
                lookahead++;
            }
        }
        return counters;
    }

    void LocalAssemblyReadDecoder::decodeMismatches(size_t clip_offset, std::string &sequence, std::string &cigar_extended) {
        uint64_t mismatchPosition = 0;
        uint64_t cigarOffset = cigar_extended.find_first_not_of(']');
        while (!container.pull(genie::GenSub::MMPOS_TERMINATOR)) {
            mismatchPosition += container.pull((genie::GenSub::MMPOS_POSITION)) + 1;
            const auto POSITION = mismatchPosition - 1 + clip_offset;
            const auto TYPE = container.pull((genie::GenSub::MMTYPE_TYPE));
            if (TYPE == genie::GenConst::MMTYPE_SUBSTITUTION) {
                const auto SUBSTITUTION = container.pull((genie::GenSub::MMTYPE_SUBSTITUTION));
                const auto SUBSTITUTION_CHAR = getAlphabetProperties(genie::AlphabetID::ACGTN).lut[SUBSTITUTION];
                sequence[POSITION] = SUBSTITUTION_CHAR;
                cigar_extended[POSITION + cigarOffset] = SUBSTITUTION_CHAR;
            } else if (TYPE == genie::GenConst::MMTYPE_INSERTION) {
                const auto INSERTION = container.pull((genie::GenSub::MMTYPE_INSERTION));
                const auto INSERTION_CHAR = getAlphabetProperties(genie::AlphabetID::ACGTN).lut[INSERTION];
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
                c = getAlphabetProperties(genie::AlphabetID::ACGTN).lut[container.pull(genie::GenSub::UREADS)];
            }
        }
    }

    std::tuple<size_t, size_t> LocalAssemblyReadDecoder::decodeClips(std::vector<std::string> &sequences,
                                           std::vector<std::string> &cigar_extended) {
        size_t num = recordCounter++;
        std::tuple<size_t, size_t> softclip_offset = {0, 0};
        if (container.isEnd(genie::GenSub::CLIPS_RECORD_ID) || num != container.peek(genie::GenSub::CLIPS_RECORD_ID)) {
            return softclip_offset;
        }
        container.pull(genie::GenSub::CLIPS_RECORD_ID);
        auto clipType = container.pull(genie::GenSub::CLIPS_TYPE);
        while (clipType != genie::GenConst::CLIPS_RECORD_END) {
            bool hardclip = clipType & 4u;
            size_t record_no = clipType & 2u ? sequences.size() - 1 : 0;
            bool end = clipType & 1u;

            if (hardclip) {
                const auto HARDCLIP_SIZE = container.pull(genie::GenSub::CLIPS_HARD_LENGTH);
                size_t cigar_position = end ? cigar_extended[record_no].size() : 0;
                cigar_extended[record_no].insert(cigar_position, HARDCLIP_SIZE, ']');
            } else {
                std::string softClip;
                auto symbol = container.pull(genie::GenSub::CLIPS_SOFT_STRING);
                while (symbol != getAlphabetProperties(genie::AlphabetID::ACGTN).lut.size()) {
                    softClip += getAlphabetProperties(genie::AlphabetID::ACGTN).lut[symbol];
                    symbol = container.pull(genie::GenSub::CLIPS_SOFT_STRING);
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
                        std::get<0>(softclip_offset) = softClip.length();  // TODO: not working for paired
                    } else {
                        std::get<1>(softclip_offset) = softClip.length();
                    }
                }
            }
            clipType = container.pull(genie::GenSub::CLIPS_TYPE);
        }

        return softclip_offset;
    }
}  // namespace mpegg_read
}  // namespace genie