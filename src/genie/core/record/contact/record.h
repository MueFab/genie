/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_CONTACT_H_
#define SRC_GENIE_CORE_RECORD_CONTACT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <boost/optional/optional.hpp>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/core/record/linked_record/linked_record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

/**
 *  @brief
 */
class ContactRecord {
 private:
    uint8_t sample_ID;
    std::string sample_name;
    uint8_t chr1_ID;
    std::string chr1_name;
    uint8_t chr2_ID;
    std::string chr2_name;
    uint64_t num_counts;
//    uint8_t num_norm_counts;
    std::vector<std::string> norm_count_names;
    std::vector<uint64_t> start_pos1;
    std::vector<uint64_t> end_pos1;
    std::vector<uint64_t> start_pos2;
    std::vector<uint64_t> end_pos2;
    std::vector<uint32_t> counts;
//    norm_count
    boost::optional<LinkRecord> link_record;

 public:
    /**
     * @brief
     */
//    Record();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_CONTACT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
