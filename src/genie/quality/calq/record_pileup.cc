/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/quality/calq/record_pileup.h"
#include <algorithm>
#include <cctype>
#include <map>
#include "genie/core/constants.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::quality::calq {

// ---------------------------------------------------------------------------------------------------------------------

RecordPileup::RecordPileup() : minPos(static_cast<uint64_t>(-1)), maxPos(static_cast<uint64_t>(0)) {}

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

void RecordPileup::addRecord(EncodingRecord &r) {
    nextRecord();

    records.emplace_back(r);

    for (size_t i = 0; i < r.cigars.size(); ++i) {
        auto seq_processed = preprocess(r.sequences[i], r.cigars[i]);
        auto qual_processed = preprocess(r.qvalues[i], r.cigars[i]);

        this->minPos = std::min(this->minPos, r.positions[i]);
        this->maxPos = std::max(this->maxPos, r.positions[i] + seq_processed.length() - 1);

        preprocessed_qvalues.back().emplace_back(std::move(qual_processed));
        preprocessed_sequences.back().emplace_back(std::move(seq_processed));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::pair<std::string, std::string> RecordPileup::getPileup(uint64_t pos) {
    UTILS_DIE_IF((pos < this->minPos || pos > this->maxPos), "Position out of range");

    std::string seqs, quals;

    for (uint64_t i = 0; i < this->records.size(); ++i) {
        for (uint64_t read_i = 0; read_i < records[i].positions.size(); ++read_i) {
            const auto pos_read = records[i].positions[read_i];
            const auto &seq = preprocessed_sequences[i][read_i];
            const auto &qual = preprocessed_qvalues[i][read_i];

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

uint64_t RecordPileup::getMinPos() const { return minPos; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t RecordPileup::getMaxPos() const { return maxPos; }

// ---------------------------------------------------------------------------------------------------------------------

void RecordPileup::nextRecord() {
    preprocessed_qvalues.emplace_back();
    preprocessed_sequences.emplace_back();
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<EncodingRecord> RecordPileup::getRecordsBefore(uint64_t pos) {
    // new vectors
    std::vector<std::vector<std::string>> new_pre_seqs;
    std::vector<std::vector<std::string>> new_pre_quals;
    std::vector<EncodingRecord> return_records;
    std::vector<EncodingRecord> new_records;

    if (pos <= this->minPos) {
        // return empty vectors
        return return_records;
    }

    if (pos > this->maxPos) {
        // return all
        this->minPos = static_cast<uint64_t>(-1);
        this->maxPos = 0;

        this->preprocessed_qvalues.clear();
        this->preprocessed_sequences.clear();
        std::swap(this->records, return_records);

        return return_records;
    }

    for (uint64_t record_i = 0; record_i < this->records.size(); ++record_i) {
        auto &record_positions = this->records[record_i].positions;
        auto &record_seqs = this->preprocessed_sequences[record_i];

        // move record to according vector
        if (isRecordBeforePos(record_positions, record_seqs, pos)) {
            return_records.emplace_back(std::move(this->records[record_i]));

        } else {
            new_records.emplace_back(std::move(this->records[record_i]));
            new_pre_quals.emplace_back(std::move(this->preprocessed_qvalues[record_i]));
            new_pre_seqs.emplace_back(std::move(this->preprocessed_sequences[record_i]));
        }
    }

    // assign new vectors to class
    this->records = std::move(new_records);
    this->preprocessed_sequences = std::move(new_pre_seqs);
    this->preprocessed_qvalues = std::move(new_pre_quals);

    // assign new min/max values
    this->minPos = this->records.front().positions.front();

    return return_records;
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<EncodingRecord> RecordPileup::getAllRecords() { return this->records; }

// ---------------------------------------------------------------------------------------------------------------------

bool RecordPileup::isRecordBeforePos(const std::vector<uint64_t> &positions,
                                     const std::vector<std::string> &preprocessed_seqs, uint64_t pos) {
    for (uint64_t read_i = 0; read_i < positions.size(); ++read_i) {
        uint64_t readMaxPos = positions[read_i] + preprocessed_seqs[read_i].size() - 1;

        if (readMaxPos >= pos) {
            return false;
        }
    }

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

bool RecordPileup::empty() { return records.empty(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::quality::calq

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
