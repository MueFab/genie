/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/read/localassembly/local-reference.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <map>
#include "genie/core/constants.h"
#include "genie/core/record/alignment_split/same-rec.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace localassembly {

// ---------------------------------------------------------------------------------------------------------------------

LocalReference::LocalReference(uint32_t _cr_buf_max_size) : cr_buf_max_size(_cr_buf_max_size), crBufSize(0) {}

// ---------------------------------------------------------------------------------------------------------------------

std::string LocalReference::preprocess(const std::string &read, const std::string &cigar) {
    std::string result;
    size_t count = 0;
    size_t read_pos = 0;

    for (char cigar_pos : cigar) {
        if (std::isdigit(cigar_pos)) {
            count *= 10;
            count += cigar_pos - '0';
            continue;
        }
        if (cigar_pos == '(' || cigar_pos == '[') {
            continue;
        }
        if (getAlphabetProperties(core::AlphabetID::ACGTN).isIncluded(cigar_pos) && count == 0) {
            result += read[read_pos++];
            continue;
        }
        switch (cigar_pos) {
            case '=':
                for (size_t i = 0; i < count; ++i) {
                    if (read_pos >= read.length()) {
                        UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
                    }
                    result += read[read_pos++];
                }
                break;

            case '+':
            case ')':
                read_pos += count;
                break;

            case ']':
                break;

            case '-':
            case '*':
            case '/':
            case '%':
                for (size_t i = 0; i < count; ++i) {
                    result += '0';
                }
                break;
            default:
                UTILS_THROW_RUNTIME_EXCEPTION("Unknown CIGAR character");
        }
        count = 0;
    }

    if (read_pos != read.length()) {
        UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
    }
    return result;
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalReference::addSingleRead(const std::string &record, const std::string &ecigar, uint64_t position) {
    sequence_positions.push_back(position);
    std::string read = preprocess(record, ecigar);
    sequences.push_back(read);
    crBufSize += (uint32_t)read.length();

    while (crBufSize > cr_buf_max_size) {
        if (sequences.size() == 1) {
            UTILS_THROW_RUNTIME_EXCEPTION("Read too long for current cr_buf_max_size");
        }
        // Erase oldest read
        crBufSize -= (uint32_t)sequences.front().length();
        sequences.erase(sequences.begin());
        sequence_positions.erase(sequence_positions.begin());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalReference::addRead(const core::record::Record &s) {
    const auto &seq1 = s.getSegments()[0].getSequence();
    const auto &cigar1 = s.getAlignments().front().getAlignment().getECigar();
    const auto pos1 = s.getAlignments().front().getPosition();
    addSingleRead(seq1, cigar1, pos1);

    if (s.getSegments().size() == 1) {
        return;
    }

    UTILS_DIE_IF(s.getAlignments().front().getAlignmentSplits().front()->getType() !=
                     core::record::AlignmentSplit::Type::SAME_REC,
                 "Only same record split alignments supported");

    const auto ptr = s.getAlignments().front().getAlignmentSplits().front().get();
    const auto &rec = dynamic_cast<const core::record::alignment_split::SameRec &>(*ptr);

    const auto &seq2 = s.getSegments()[1].getSequence();
    const auto &cigar2 = rec.getAlignment().getECigar();
    const auto pos2 = s.getAlignments().front().getPosition() + rec.getDelta();
    addSingleRead(seq2, cigar2, pos2);
}

// ---------------------------------------------------------------------------------------------------------------------

std::string LocalReference::generateRef(uint32_t offset, uint32_t len) {
    std::string ref;
    for (uint32_t i = offset; i < offset + len; ++i) {
        ref += majorityVote(i);
    }
    return ref;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t LocalReference::lengthFromCigar(const std::string &cigar) {
    uint32_t len = 0;
    uint32_t count = 0;
    for (char cigar_pos : cigar) {
        if (std::isdigit(cigar_pos)) {
            count *= 10;
            count += cigar_pos - '0';
            continue;
        }
        if (cigar_pos == '(' || cigar_pos == '[') {
            continue;
        }
        if (getAlphabetProperties(core::AlphabetID::ACGTN).isIncluded(cigar_pos) && count == 0) {
            len += 1;
            continue;
        }
        switch (cigar_pos) {
            case '=':
            case '-':
            case '*':
            case '/':
            case '%':
                len += count;
                count = 0;
                break;

            case '+':
            case ')':
            case ']':
                count = 0;
                break;
            default:
                UTILS_THROW_RUNTIME_EXCEPTION("Unknown CIGAR character");
        }
        count = 0;
    }
    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string LocalReference::getReference(uint32_t abs_pos, const std::string &cigar) {
    return generateRef(abs_pos, lengthFromCigar(cigar));
}

// ---------------------------------------------------------------------------------------------------------------------

std::string LocalReference::getReference(uint32_t abs_pos, uint32_t len) { return generateRef(abs_pos, len); }

// ---------------------------------------------------------------------------------------------------------------------

char LocalReference::majorityVote(uint32_t abs_position) {
    std::map<char, uint16_t> votes;

    // Collect all alignments
    for (size_t i = 0; i < sequences.size(); ++i) {
        int64_t distance = abs_position - sequence_positions[i];
        if (distance >= 0 && uint64_t(distance) < sequences[i].length()) {
            char c = sequences[i][distance];
            if (c != '0') {
                votes[c]++;
            }
        }
    }

    // Find max
    char max = '\0';
    uint16_t max_value = 0;
    for (auto &v : votes) {
        if (max_value < v.second) {
            max_value = v.second;
            max = v.first;
        } else if (max_value == v.second) {
            max = getAlphabetProperties(core::AlphabetID::ACGTN)
                      .lut[std::min(getAlphabetProperties(core::AlphabetID::ACGTN).inverseLut[max],
                                    getAlphabetProperties(core::AlphabetID::ACGTN).inverseLut[v.first])];
        }
    }

    return max;
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalReference::printWindow() const {
    uint64_t minPos = getWindowBorder();

    for (size_t i = 0; i < sequences.size(); ++i) {
        uint64_t totalOffset = sequence_positions[i] - minPos;
        for (size_t s = 0; s < totalOffset; ++s) {
            std::cerr << ".";
        }
        std::cerr << sequences[i] << std::endl;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t LocalReference::getWindowBorder() const {
    uint64_t minPos = std::numeric_limits<uint64_t>::max();
    for (auto &p : sequence_positions) {
        minPos = std::min(minPos, p);
    }
    return minPos;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t LocalReference::getMaxBufferSize() const { return cr_buf_max_size; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace localassembly
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
