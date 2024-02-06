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

ContactRecord::ContactRecord()
    : sample_ID(0),
      sample_name(),
      chr1_ID(0),
      chr1_name(),
      chr2_ID(0),
      chr2_name(),
      norm_count_names(),
      start_pos1(),
      end_pos1(),
      start_pos2(),
      end_pos2(),
      counts(),
      link_record()
{}

// ---------------------------------------------------------------------------------------------------------------------

ContactRecord::ContactRecord(util::BitReader &reader){
    // Sample
    sample_ID = reader.readBypassBE<uint8_t>();
    sample_name.resize(reader.readBypassBE<uint8_t>());
    reader.readBypass(&sample_name[0], sample_name.size());

    // chr1
    chr1_ID = reader.readBypassBE<uint8_t>();
    chr1_name.resize(reader.readBypassBE<uint8_t>());
    reader.readBypass(&chr1_name[0], chr1_name.size());

    // chr2
    chr2_ID = reader.readBypassBE<uint8_t>();
    chr2_name.resize(reader.readBypassBE<uint8_t>());
    reader.readBypass(&chr2_name[0], chr2_name.size());

    // num_counts and num_norm_counts
    auto num_counts = reader.readBypassBE<uint64_t>();
    auto num_norm_counts = reader.readBypassBE<uint8_t>();

    //TODO @Yeremia: Implement this!
    UTILS_DIE_IF(num_norm_counts != 0, "Not yet implemented for num_norm_counts > 0");
    for (auto i = 0; i < num_norm_counts; i++){
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

    auto link_record_flag = reader.readBypassBE<uint8_t>();
    UTILS_DIE_IF(link_record_flag, "Not yet implemented for link_record!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactRecord::getSampleID() const {return sample_ID;}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ContactRecord::getSampleName() const {return sample_name;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactRecord::getChr1ID() const {return chr1_ID;}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ContactRecord::getChr1Name() const {return chr1_name;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactRecord::getChr2ID() const {return chr2_ID;}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ContactRecord::getChr2Name() const {return chr2_name;}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ContactRecord::getNumCounts() const {return counts.size();}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactRecord::getNumNormCounts() const {return norm_count_names.size();}

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

// TODO (Yeremia): Implement this
//void ContactRecord::write(util::BitWriter &writer) const {}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ContactRecord::inferChr1Length(){
    uint64_t max_pos = 0;
    for (auto& pos: end_pos1){
        if (pos > max_pos){
            max_pos = pos;
        }
    }
    return max_pos;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ContactRecord::inferChr2Length(){
    uint64_t max_pos = 0;
    for (auto& pos: end_pos2){
        if (pos > max_pos){
            max_pos = pos;
        }
    }
    return max_pos;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactRecord::inferInterval(){
    uint32_t interval = 0;
    for (auto i = 0; i<getNumCounts(); i++){
        bool updated = false;

        uint32_t new_interval = end_pos1[i] - start_pos1[i];
        if (interval < new_interval){
            interval = new_interval;
            updated = true;
        }

        new_interval = end_pos2[i] - start_pos2[i];
        if (interval < new_interval){
            interval = new_interval;
            updated = true;
        }

        if (!updated){
            break;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
