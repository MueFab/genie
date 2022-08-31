/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <map>
#include "genie/core/constants.h"
#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/quality/calq/record_pileup.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace calq {

// ---------------------------------------------------------------------------------------------------------------------

RecordPileup::RecordPileup() : minPos(-1), maxPos(0) {}

// ---------------------------------------------------------------------------------------------------------------------

std::string RecordPileup::preprocess(const std::string &read, const std::string &cigar) {
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

void RecordPileup::addSingleRead(const std::string &seq, const std::string &qual, const std::string &ecigar,
                                   uint64_t position) {
    auto seq_processed = preprocess(seq, ecigar);
    auto qual_processed = preprocess(qual, ecigar);

    UTILS_DIE_IF(seq_processed.empty(), "Empty read");

    this->minPos = std::min(this->minPos, position);
    this->maxPos = std::max(this->maxPos, position + seq_processed.length() - 1);

    sequences.back().emplace_back(std::move(seq_processed));
    qualities.back().emplace_back(std::move(qual_processed));
    sequence_positions.back().push_back(position);
}

// ---------------------------------------------------------------------------------------------------------------------

std::pair<std::string, std::string> RecordPileup::getPileup(uint64_t pos) {
    UTILS_DIE_IF((pos < this->minPos || pos > this->maxPos), "Position out of range");

    std::string seqs, quals;

    for (uint64_t i = 0; i < this->sequence_positions.size(); ++i) {
        for (uint64_t read_i = 0; read_i < this->sequence_positions[i].size(); ++read_i) {
            const auto pos_read = this->sequence_positions[i][read_i];
            const auto &seq = this->sequences[i][read_i];
            const auto &qual = this->qualities[i][read_i];

            if ((pos < pos_read) || (pos > pos_read + seq.size() - 1)) {
                continue;
            }

            char base = seq[pos - pos_read];

            if (base != '0') {
                seqs.push_back(seq[pos - pos_read]);
                quals.push_back(qual[pos - pos_read]);
            }
        }
    }
    return std::make_pair(seqs, quals);
}

// ---------------------------------------------------------------------------------------------------------------------

void RecordPileup::addRead(const core::record::Record &s) {
    sequences.emplace_back();
    sequence_positions.emplace_back();
    qualities.emplace_back();

    const auto &seq1 = s.getSegments()[0].getSequence();
    const auto &qual1 = s.getSegments()[0].getQualities().front();
    const auto &cigar1 = s.getAlignments().front().getAlignment().getECigar();
    const auto pos1 = s.getAlignments().front().getPosition();

    addSingleRead(seq1, qual1, cigar1, pos1);

    if (s.getSegments().size() != 1) {
        return;
    }

    UTILS_DIE_IF(s.getAlignments().front().getAlignmentSplits().front()->getType() !=
                     core::record::AlignmentSplit::Type::SAME_REC,
                 "Only same record split alignments supported");

    const auto ptr = s.getAlignments().front().getAlignmentSplits().front().get();
    const auto &rec = dynamic_cast<const core::record::alignment_split::SameRec &>(*ptr);

    const auto &seq2 = s.getSegments()[1].getSequence();
    const auto &qual2 = s.getSegments()[1].getQualities().front();
    const auto &cigar2 = rec.getAlignment().getECigar();
    const auto pos2 = s.getAlignments().front().getPosition() + rec.getDelta();

    addSingleRead(seq2, qual2, cigar2, pos2);
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t RecordPileup::lengthFromCigar(const std::string &cigar) {
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

uint64_t RecordPileup::getMinPos() const { return minPos; }

// ---------------------------------------------------------------------------------------------------------------------

void RecordPileup::nextRecord() {
    sequences.emplace_back();
    sequence_positions.emplace_back();
    qualities.emplace_back();
}

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<std::vector<std::vector<uint64_t>>, std::vector<std::vector<std::string>>,
           std::vector<std::vector<std::string>>>
RecordPileup::getRecordsBefore(uint64_t pos) {
    // new vectors
    std::vector<std::vector<std::string>> return_seqs, return_quals, new_seqs, new_quals;
    std::vector<std::vector<uint64_t>> return_positions, new_positions;

    if (pos <= this->minPos) {
        // return empty vectors
        return std::make_tuple(return_positions, return_seqs, return_quals);
    }

    if (pos > this->maxPos) {
        // return all
        this->minPos = -1;
        this->maxPos = 0;

        std::swap(this->sequence_positions, return_positions);
        std::swap(this->sequences, return_seqs);
        std::swap(this->qualities, return_quals);

        return std::make_tuple(return_positions, return_seqs, return_quals);
    }

    for (uint64_t record_i = 0; record_i < this->sequence_positions.size(); ++record_i) {
        auto &record_positions = this->sequence_positions[record_i];
        auto &record_seqs = this->sequences[record_i];

        // move record to according vector
        if (isRecordBeforePos(record_positions, record_seqs, pos)) {
            return_positions.emplace_back(std::move(this->sequence_positions[record_i]));
            return_seqs.emplace_back(std::move(this->sequences[record_i]));
            return_quals.emplace_back(std::move(this->qualities[record_i]));
        } else {
            new_positions.emplace_back(std::move(this->sequence_positions[record_i]));
            new_seqs.emplace_back(std::move(this->sequences[record_i]));
            new_quals.emplace_back(std::move(this->qualities[record_i]));
        }
    }

    // assign new min/max values
    this->minPos = -1;
    for (auto &recordPositions : new_positions) {
        for (auto read_pos : recordPositions) {
            this->minPos = std::min(this->minPos, read_pos);
        }
    }
    if (new_positions.empty()) {
        this->maxPos = 0;
    }

    // assign new vectors to class
    this->sequence_positions = std::move(new_positions);
    this->sequences = std::move(new_seqs);
    this->qualities = std::move(new_quals);

    return std::make_tuple(return_positions, return_seqs, return_quals);
}

// ---------------------------------------------------------------------------------------------------------------------

bool RecordPileup::isRecordBeforePos(const std::vector<uint64_t> &positions, const std::vector<std::string> &seqs,
                                       uint64_t pos) {
    for (uint64_t read_i = 0; read_i < positions.size(); ++read_i) {
        uint64_t readMaxPos = positions[read_i] + seqs[read_i].size() - 1;

        if (readMaxPos >= pos) {
            return false;
        }
    }

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

bool RecordPileup::empty() { return sequence_positions.empty() && sequences.empty() && qualities.empty(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace calq
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
