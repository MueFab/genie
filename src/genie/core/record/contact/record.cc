/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
#include "record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

ContactRecord::ContactRecord(
    uint8_t _sample_ID,
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
    sample_ID = reader.readBypassBE<uint8_t>();
    sample_name.resize(reader.readBypassBE<uint8_t>());
    reader.readBypass(&sample_name[0], sample_name.size());
    bin_size = reader.readBypassBE<uint32_t>();

    // chr1
    chr1_ID = reader.readBypassBE<uint8_t>();
    chr1_name.resize(reader.readBypassBE<uint8_t>());
    reader.readBypass(&chr1_name[0], chr1_name.size());
    chr1_length = reader.read<uint64_t>();

    // chr2
    chr2_ID = reader.readBypassBE<uint8_t>();
    chr2_name.resize(reader.readBypassBE<uint8_t>());
    reader.readBypass(&chr2_name[0], chr2_name.size());
    chr2_length = reader.read<uint64_t>();

    // num_counts and num_norm_counts
    auto num_counts = reader.readBypassBE<uint64_t>();
    auto num_norm_counts = reader.readBypassBE<uint8_t>();

//    TODO @Yeremia: Implement this!
//    UTILS_DIE_IF(num_norm_counts != 0, "Not yet implemented for num_norm_counts > 0");
    norm_count_names.resize(num_norm_counts);
    for (auto i = 0; i < num_norm_counts; i++){
        norm_count_names[i].resize(reader.readBypassBE<uint8_t>());
        reader.readBypass(&norm_count_names[i][0], norm_count_names[i].size());
    }

    start_pos1.resize(num_counts);
    for (size_t i = 0; i< num_counts; i++){
        start_pos1[i] = reader.readBypassBE<uint64_t>();
    }

    end_pos1.resize(num_counts);
    for (size_t i = 0; i< num_counts; i++){
        end_pos1[i] = reader.readBypassBE<uint64_t>();
    }

    start_pos2.resize(num_counts);
    for (size_t i = 0; i< num_counts; i++){
        start_pos2[i] = reader.readBypassBE<uint64_t>();
    }

    end_pos2.resize(num_counts);
    for (size_t i = 0; i< num_counts; i++){
        end_pos2[i] = reader.readBypassBE<uint64_t>();
    }

    counts.resize(num_counts);
    for (size_t i = 0; i< num_counts; i++){
        counts[i] = reader.readBypassBE<uint32_t>();
    }

    norm_counts.resize(num_norm_counts);
    for (size_t j = 0; j < num_norm_counts; j++){
        norm_counts[j].resize(num_counts);
        for (size_t i = 0; i< num_counts; i++){
            norm_counts[j][i] = reader.readBypassBE<double_t>();
        }
    }

    auto link_record_flag = reader.readBypassBE<uint8_t>();
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

ContactRecord &ContactRecord::operator=(ContactRecord &&rec) noexcept {
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

uint8_t ContactRecord::getSampleID() const {return sample_ID;}

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

uint64_t ContactRecord::getNumCounts() const {return counts.size();}

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

// TODO (Yeremia): Implement this
//void ContactRecord::write(util::BitWriter &writer) const {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
