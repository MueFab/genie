/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "record.h"
#include <algorithm>
#include <string>
#include <utility>
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

ContactRecord::ContactRecord(
    uint16_t _sample_ID,
    std::string&& _sample_name,
    uint32_t _bin_size,
    uint8_t _chr1_ID,
    std::string&& _chr1_name,
    uint64_t _chr1_length,
    uint8_t _chr2_ID,
    std::string&& _chr2_name,
    uint64_t _chr2_length,
    std::vector<std::string>&& _norm_count_names,
    std::vector<uint64_t>&& _start_pos1,
    std::vector<uint64_t>&& _end_pos1,
    std::vector<uint64_t>&& _start_pos2,
    std::vector<uint64_t>&& _end_pos2,
    std::vector<uint32_t>&& _counts)
    : sample_ID(_sample_ID),
      sample_name(std::move(_sample_name)),
      bin_size(_bin_size),
      chr1_ID(_chr1_ID),
      chr1_name(std::move(_chr1_name)),
      chr1_length(_chr1_length),
      chr2_ID(_chr2_ID),
      chr2_name(std::move(_chr2_name)),
      chr2_length(_chr2_length),
      norm_count_names(std::move(_norm_count_names)),
      start_pos1(std::move(_start_pos1)),
      end_pos1(std::move(_end_pos1)),
      start_pos2(std::move(_start_pos2)),
      end_pos2(std::move(_end_pos2)),
      counts(std::move(_counts)) {}

// ---------------------------------------------------------------------------------------------------------------------

//ContactRecord::ContactRecord(const ContactRecord& rec) {*this = rec;}

// ---------------------------------------------------------------------------------------------------------------------

//ContactRecord::ContactRecord(ContactRecord&& rec) noexcept {*this = std::move(rec); }

// ---------------------------------------------------------------------------------------------------------------------

ContactRecord::ContactRecord(util::BitReader &reader){
    // Sample
    sample_ID = reader.ReadAlignedInt<uint16_t>();
    sample_name.resize(reader.ReadAlignedInt<uint8_t>());
    reader.ReadAlignedBytes(&sample_name[0], sample_name.size());
    bin_size = reader.ReadAlignedInt<uint32_t>();

    // chr1
    chr1_ID = reader.ReadAlignedInt<uint8_t>();
    chr1_name.resize(reader.ReadAlignedInt<uint8_t>());
    reader.ReadAlignedBytes(&chr1_name[0], chr1_name.size());
    chr1_length = reader.Read<uint64_t>();

    // chr2
    chr2_ID = reader.ReadAlignedInt<uint8_t>();
    chr2_name.resize(reader.ReadAlignedInt<uint8_t>());
    reader.ReadAlignedBytes(&chr2_name[0], chr2_name.size());
    chr2_length = reader.Read<uint64_t>();

    // num_entries and num_norm_counts
    auto num_entries = reader.ReadAlignedInt<uint64_t>();
    auto num_norm_counts = reader.ReadAlignedInt<uint8_t>();

    norm_count_names.resize(num_norm_counts);
    for (auto i = 0; i < num_norm_counts; i++){
        norm_count_names[i].resize(reader.ReadAlignedInt<uint8_t>());
        reader.ReadAlignedBytes(&norm_count_names[i][0], norm_count_names[i].size());
    }

    start_pos1.resize(num_entries);
    for (size_t i = 0; i< num_entries; i++){
        start_pos1[i] = reader.ReadAlignedInt<uint64_t>();
    }

    end_pos1.resize(num_entries);
    for (size_t i = 0; i< num_entries; i++){
        end_pos1[i] = reader.ReadAlignedInt<uint64_t>();
    }

    start_pos2.resize(num_entries);
    for (size_t i = 0; i< num_entries; i++){
        start_pos2[i] = reader.ReadAlignedInt<uint64_t>();
    }

    end_pos2.resize(num_entries);
    for (size_t i = 0; i< num_entries; i++){
        end_pos2[i] = reader.ReadAlignedInt<uint64_t>();
    }

    counts.resize(num_entries);
    for (size_t i = 0; i< num_entries; i++){
        counts[i] = reader.ReadAlignedInt<uint32_t>();
    }

    norm_counts.resize(num_norm_counts);
    for (size_t j = 0; j < num_norm_counts; j++){
        norm_counts[j].resize(num_entries);
        for (size_t i = 0; i< num_entries; i++){
            norm_counts[j][i] = reader.ReadAlignedInt<double_t>();
        }
    }

    auto link_record_flag = reader.ReadAlignedInt<uint8_t>();
    UTILS_DIE_IF(link_record_flag, "Not yet implemented for link_record!");
}

// ---------------------------------------------------------------------------------------------------------------------

ContactRecord &ContactRecord::operator=(const ContactRecord &rec) {
    if (this == &rec) {
        return *this;
    }

    this->sample_ID = rec.sample_ID;
    this->sample_name = rec.sample_name;
    this->bin_size = rec.bin_size;
    this->chr1_ID = rec.chr1_ID;
    this->chr1_name = rec.chr1_name;
    this->chr1_length = rec.chr1_length;
    this->chr2_ID = rec.chr2_ID;
    this->chr2_name = rec.chr2_name;
    this->chr2_length = rec.chr2_length;
    this->norm_count_names = rec.norm_count_names;
    this->start_pos1 = rec.start_pos1;
    this->end_pos1 = rec.end_pos1;
    this->start_pos2 = rec.start_pos2;
    this->end_pos2 = rec.end_pos2;
    this->counts = rec.counts;

    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

ContactRecord& ContactRecord::operator=(ContactRecord &&rec) noexcept {
    this->sample_ID = rec.sample_ID;
    this->sample_name = std::move(rec.sample_name);
    this->bin_size = rec.bin_size;
    this->chr1_ID = rec.chr1_ID;
    this->chr1_name = std::move(rec.chr1_name);
    this->chr1_length = rec.chr1_length;
    this->chr2_ID = rec.chr2_ID;
    this->chr2_name = std::move(rec.chr2_name);
    this->chr2_length = rec.chr2_length;
    this->norm_count_names = std::move(rec.norm_count_names);
    this->start_pos1 = std::move(rec.start_pos1);
    this->end_pos1 = std::move(rec.end_pos1);
    this->start_pos2 = std::move(rec.start_pos2);
    this->end_pos2 = std::move(rec.end_pos2);
    this->counts = std::move(rec.counts);

    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

bool ContactRecord::operator==(const ContactRecord& other) {
    bool ret =
        sample_ID == other.sample_ID &&
        sample_name == other.sample_name &&
        bin_size == other.bin_size &&
        chr1_ID == other.chr1_ID &&
        chr1_name == other.chr1_name &&
        chr1_length == other.chr1_length &&
        chr2_ID == other.chr2_ID &&
        chr2_name == other.chr2_name &&
        chr2_length == other.chr2_length &&
//        norm_count_names.size() == other.norm_count_names.size() &&
        norm_count_names == other.norm_count_names &&
//        start_pos1.size() == other.start_pos1.size() &&
        start_pos1 == other.start_pos1 &&
//        end_pos1.size() == other.end_pos1.size() &&
        end_pos1 == other.end_pos1 &&
//        start_pos2.size() == other.start_pos2.size() &&
        start_pos2 == other.start_pos2 &&
//        end_pos2.size() == other.end_pos2.size() &&
        end_pos2 == other.end_pos2 &&
//        counts.size() == other.counts.size() &&
        counts == other.counts &&
//        norm_counts.size() == other.norm_counts.size() &&
        norm_counts == other.norm_counts &&
        link_record.has_value() == other.link_record.has_value();

    //TODO(yeremia): this this "const" problem
//    if (ret){
//        const auto& this_link_record = link_record.value();
//        const auto& other_link_record = other.link_record.value();
//        ret = this_link_record == other_link_record;
//    }

    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::transposeCM(){
    uint8_t chr_ID = chr1_ID;
    std::string chr_name = std::move(chr1_name);
    uint64_t chr_length = chr1_length;

    chr1_ID = chr2_ID;
    chr1_name = std::move(chr2_name);
    chr1_length = chr2_length;
    chr2_ID = chr_ID;
    chr2_name = std::move(chr_name);
    chr2_length = chr_length;

    std::vector<uint64_t> start_pos = std::move(start_pos1);
    std::vector<uint64_t> end_pos = std::move(end_pos1);
    start_pos1 = std::move(start_pos2);
    end_pos1 = std::move(end_pos2);
    start_pos2 = std::move(start_pos1);
    end_pos2 = std::move(end_pos1);
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t ContactRecord::getSampleID() const {return sample_ID;}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ContactRecord::getSampleName() const {return sample_name;}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactRecord::getBinSize() const {return bin_size;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactRecord::getChr1ID() const {return chr1_ID;}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ContactRecord::getChr1Name() const {return chr1_name;}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ContactRecord::getChr1Length() const {return chr1_length;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactRecord::getChr2ID() const {return chr2_ID;}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ContactRecord::getChr2Name() const {return chr2_name;}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ContactRecord::getChr2Length() const {return chr2_length;}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ContactRecord::getNumEntries() const {return counts.size();}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactRecord::getNumNormCounts() const {return static_cast<uint8_t>(norm_count_names.size());}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& ContactRecord::getNormCountNames() const{ return norm_count_names;}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint64_t>& ContactRecord::getStartPos1() const {return start_pos1;}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint64_t>& ContactRecord::getEndPos1() const {return end_pos1;}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint64_t>& ContactRecord::getStartPos2() const {return start_pos2;}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint64_t>& ContactRecord::getEndPos2() const {return end_pos2;}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint32_t>& ContactRecord::getCounts() const {return counts;}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<double_t>>& ContactRecord::getNormCounts() const { return norm_counts;}

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::setSampleID(uint16_t _sample_ID){ sample_ID = _sample_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::setSampleName(std::string&& _sample_name){ sample_name = std::move(_sample_name); }

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::setBinSize(uint32_t _bin_size){ bin_size = _bin_size; }

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::setChr1ID(uint8_t _chr1_ID){ chr1_ID = _chr1_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::setChr1Name(std::string&& _chr1_name){ chr1_name = std::move(_chr1_name); }

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::setChr1Length(uint64_t _chr1_len){ chr1_length = _chr1_len;}

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::setChr2ID(uint8_t _chr2_ID){ chr2_ID = _chr2_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::setChr2Name(std::string&& _chr2_name){ chr2_name = std::move(_chr2_name); }

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::setChr2Length(uint64_t _chr2_len){ chr2_length = _chr2_len; }

// ---------------------------------------------------------------------------------------------------------------------

void ContactRecord::SetCMValues(
    std::vector<uint64_t>&& _start_pos1,
    std::vector<uint64_t>&& _end_pos1,
    std::vector<uint64_t>&& _start_pos2,
    std::vector<uint64_t>&& _end_pos2,
    std::vector<uint32_t>&& _counts
){
    UTILS_DIE_IF(
        _counts.size() != _start_pos1.size() ||
            _counts.size()!= _end_pos1.size() ||
            _counts.size()!= _start_pos2.size() ||
            _counts.size() != _end_pos2.size(),
        "Invalid length of CM values"
    );

    start_pos1 = std::move(_start_pos1);
    end_pos1 = std::move(_end_pos1);
    start_pos2 = std::move(_start_pos2);
    end_pos2 = std::move(_end_pos2);
    counts = std::move(_counts);
}

// ---------------------------------------------------------------------------------------------------------------------

// TODO (Yeremia): Implement this
void ContactRecord::write(util::BitWriter &writer) const {
    // Sample
    writer.writeBypassBE(sample_ID);
    writer.writeBypassBE(static_cast<uint8_t>(sample_name.length()));
    writer.writeBypass(sample_name.data(), sample_name.length());
    writer.writeBypassBE(bin_size);

    // chr1
    writer.writeBypassBE(chr1_ID);
    writer.writeBypassBE(static_cast<uint8_t>(chr1_name.length()));
    writer.writeBypass(chr1_name.data(), chr1_name.length());
    writer.writeBypassBE(chr1_length);

    // chr2
    writer.writeBypassBE(chr2_ID);
    writer.writeBypassBE(static_cast<uint8_t>(chr2_name.length()));
    writer.writeBypass(chr2_name.data(), chr2_name.length());
    writer.writeBypassBE(chr2_length);

    // num_entries and num_norm_counts
    writer.writeBypassBE(getNumEntries());
    writer.writeBypassBE(getNumNormCounts());

    for (auto i = 0; i < getNumNormCounts(); i++){
        writer.writeBypassBE(static_cast<uint8_t>(norm_count_names[i].length()));
        writer.writeBypass(norm_count_names[i].data(), norm_count_names[i].length());
    }

    for (const auto& v: start_pos1){
        writer.writeBypassBE(v);
    }

    for (const auto& v: end_pos1){
        writer.writeBypassBE(v);
    }

    for (const auto& v: start_pos2){
        writer.writeBypassBE(v);
    }

    for (const auto& v: end_pos2){
        writer.writeBypassBE(v);
    }

    for (const auto& v: counts){
        writer.writeBypassBE(v);
    }

    for (const auto& norm_count_vals: norm_counts){
        for (const auto& v: norm_count_vals){
            writer.writeBypassBE(v);
        }
    }

    //TODO(yeremia): fix the flag
    //    auto link_record_flag = reader.ReadAlignedInt<uint8_t>();
    writer.writeBypassBE(static_cast<uint8_t>(0));
    //    UTILS_DIE_IF(link_record_flag, "Not yet implemented for link_record!");
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
