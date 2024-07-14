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
#include <optional>
#include <boost/optional/optional.hpp>
#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/core/record/linked_record/linked_record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

class ContactRecord {
 private:
    uint8_t sample_ID;
    std::string sample_name;
    uint32_t bin_size;
    uint8_t chr1_ID;
    std::string chr1_name;
    uint64_t chr1_length;
    uint8_t chr2_ID;
    std::string chr2_name;
    uint64_t chr2_length;
    std::vector<std::string> norm_count_names;
    std::vector<uint64_t> start_pos1;
    std::vector<uint64_t> end_pos1;
    std::vector<uint64_t> start_pos2;
    std::vector<uint64_t> end_pos2;
    std::vector<uint32_t> counts;
    std::vector<std::vector<double_t>> norm_counts;
    std::optional<LinkRecord> link_record;

 public:
    ContactRecord() = default;

    ContactRecord(ContactRecord&& rec) noexcept = default;

    ContactRecord(const ContactRecord& rec) = default;

    ContactRecord(
        uint16_t sample_ID,
        std::string&& sample_name,
        uint32_t bin_size,
        uint8_t chr1_ID,
        std::string&& chr1_name,
        uint64_t chr1_length,
        uint8_t chr2_ID,
        std::string&& chr2_name,
        uint64_t chr2_length,
        std::vector<std::string>&& norm_count_names,
        std::vector<uint64_t>&& start_pos1,
        std::vector<uint64_t>&& end_pos1,
        std::vector<uint64_t>&& start_pos2,
        std::vector<uint64_t>&& end_pos2,
        std::vector<uint32_t>&& counts
    );

    explicit ContactRecord(util::BitReader& reader);

    ~ContactRecord() = default;

    ContactRecord& operator=(const ContactRecord& rec);

    ContactRecord& operator=(ContactRecord&& rec) noexcept;

    bool operator==(const ContactRecord& other);

    void transposeCM();

    uint16_t getSampleID() const;

    const std::string& getSampleName() const;

    uint32_t getBinSize() const;

    uint8_t getChr1ID() const;

    const std::string& getChr1Name() const;

    uint64_t getChr1Length() const;

    uint8_t getChr2ID() const;

    const std::string& getChr2Name() const;

     uint64_t getChr2Length() const;

     uint64_t getNumEntries() const;

     uint8_t getNumNormCounts() const;

     const std::vector<std::string>& getNormCountNames() const;

     const std::vector<uint64_t>& getStartPos1() const;

     const std::vector<uint64_t>& getEndPos1() const;

     const std::vector<uint64_t>& getStartPos2() const;

     const std::vector<uint64_t>& getEndPos2() const;

     const std::vector<uint32_t>& getCounts() const;

     const std::vector<std::vector<double_t>>& getNormCounts() const;

     void setSampleID(uint16_t _sample_ID);

     void setSampleName(std::string&& _sample_name);

     void setBinSize(uint32_t _bin_size);

     void setChr1ID(uint8_t _chr1_ID);

     void setChr1Name(std::string&& _chr1_name);

     void setChr1Length(uint64_t _chr1_len);

     void setChr2ID(uint8_t _chr2_ID);

     void setChr2Name(std::string&& _chr2_name);

     void setChr2Length(uint64_t _chr2_len);

     void SetCMValues(
         std::vector<uint64_t>&& _start_pos1,
         std::vector<uint64_t>&& _end_pos1,
         std::vector<uint64_t>&& _start_pos2,
         std::vector<uint64_t>&& _end_pos2,
         std::vector<uint32_t>&& _counts
     );

     void write(util::BitWriter &writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_CONTACT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
