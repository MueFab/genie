/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "reference-encoder.h"
#include <format/mpegg_rec/alignment-container.h>
#include <format/mpegg_rec/mpegg-record.h>
#include <format/mpegg_rec/segment.h>
#include <limits>
#include <map>
#include "util/exceptions.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace lae {

// ---------------------------------------------------------------------------------------------------------------------

LocalAssemblyReferenceEncoder::LocalAssemblyReferenceEncoder(uint32_t _cr_buf_max_size)
    : cr_buf_max_size(_cr_buf_max_size), crBufSize(0) {}

// ---------------------------------------------------------------------------------------------------------------------

std::string LocalAssemblyReferenceEncoder::preprocess(const std::string &read, const std::string &cigar) {
    std::string result;
    size_t count = 0;
    size_t read_pos = 0;

    for (size_t cigar_pos = 0; cigar_pos < cigar.length(); ++cigar_pos) {
        if (cigar[cigar_pos] >= '0' && cigar[cigar_pos] <= '9') {
            count *= 10;
            count += cigar[cigar_pos] - '0';
            continue;
        }
        switch (cigar[cigar_pos]) {
            case '=':
            case 'A':
            case 'C':
            case 'T':
            case 'G':
            case 'N':
                for (size_t i = 0; i < count; ++i) {
                    if (read_pos >= read.length()) {
                        UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
                    }
                    result += read[read_pos++];
                }
                break;

            case '+':
                read_pos += count;
                break;

            case '-':
            case '*':
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

void LocalAssemblyReferenceEncoder::addRead(const format::mpegg_rec::MpeggRecord *s) {
    sequence_positions.push_back(s->getAlignment(0)->getPosition());
    std::string read =
        preprocess(*s->getRecordSegment(0)->getSequence(), *s->getAlignment(0)->getAlignment()->getECigar());
    sequences.push_back(read);
    crBufSize += read.length();

    while (crBufSize > cr_buf_max_size) {
        if (sequences.size() == 1) {
            UTILS_THROW_RUNTIME_EXCEPTION("Read too long for current cr_buf_max_size");
        }
        // Erase oldest read
        crBufSize -= sequences.front().length();
        sequences.erase(sequences.begin());
        sequence_positions.erase(sequence_positions.begin());
    }

    if (s->getNumberOfRecords() == 1) {
        return;
    }

    const format::mpegg_rec::SplitAlignmentSameRec *rec;
    if (s->getAlignment(0)->getSplitAlignment(0)->getType() ==
        format::mpegg_rec::SplitAlignment::SplitAlignmentType::SAME_REC) {
        rec = dynamic_cast<const format::mpegg_rec::SplitAlignmentSameRec *>(s->getAlignment(0)->getSplitAlignment(0));
    } else {
        UTILS_DIE("Only same record split alignments supported");
    }
    sequence_positions.push_back(s->getAlignment(0)->getPosition() + rec->getDelta());
    read = preprocess(*s->getRecordSegment(1)->getSequence(), *rec->getAlignment()->getECigar());
    sequences.push_back(read);
    crBufSize += read.length();

    while (crBufSize > cr_buf_max_size) {
        if (sequences.size() == 1) {
            UTILS_THROW_RUNTIME_EXCEPTION("Read too long for current cr_buf_max_size");
        }
        // Erase oldest read
        crBufSize -= sequences.front().length();
        sequences.erase(sequences.begin());
        sequence_positions.erase(sequence_positions.begin());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::string LocalAssemblyReferenceEncoder::generateRef(uint32_t offset, uint32_t len) {
    std::string ref;
    for (size_t i = offset; i < offset + len; ++i) {
        ref += majorityVote(i);
    }
    return ref;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t LocalAssemblyReferenceEncoder::lengthFromCigar(const std::string &cigar) {
    uint32_t len = 0;
    uint32_t count = 0;
    for (size_t cigar_pos = 0; cigar_pos < cigar.length(); ++cigar_pos) {
        if (cigar[cigar_pos] >= '0' && cigar[cigar_pos] <= '9') {
            count *= 10;
            count += cigar[cigar_pos] - '0';
            continue;
        }
        switch (cigar[cigar_pos]) {
            case '=':
            case '-':
                len += count;
                count = 0;
                break;

            case 'I':
            case 'S':
            case 'P':
            case 'H':
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

std::string LocalAssemblyReferenceEncoder::getReference(uint32_t abs_pos, const std::string &cigar) {
    return generateRef(abs_pos, lengthFromCigar(cigar));
}

// ---------------------------------------------------------------------------------------------------------------------

std::string LocalAssemblyReferenceEncoder::getReference(uint32_t abs_pos, uint32_t len) {
    return generateRef(abs_pos, len);
}

// ---------------------------------------------------------------------------------------------------------------------

char LocalAssemblyReferenceEncoder::majorityVote(uint32_t abs_position) {
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
            max = std::min(max, v.first);
        }
    }

    return max;
}

// ---------------------------------------------------------------------------------------------------------------------

void LocalAssemblyReferenceEncoder::printWindow() const {
    uint64_t minPos = getWindowBorder();

    for (size_t i = 0; i < sequences.size(); ++i) {
        uint64_t totalOffset = sequence_positions[i] - minPos;
        for (size_t s = 0; s < totalOffset; ++s) {
            std::cout << ".";
        }
        std::cout << sequences[i] << std::endl;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t LocalAssemblyReferenceEncoder::getWindowBorder() const {
    uint64_t minPos = std::numeric_limits<uint64_t>::max();
    for (auto &p : sequence_positions) {
        minPos = std::min(minPos, p);
    }
    return minPos;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace lae

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------