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

/**
 *  @brief
 */
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
    std::optional<LinkRecord> link_record;

 public:
    /**
     * @brief
     */
    ContactRecord();

    /**
     *
     */
    ContactRecord(
        uint8_t sample_ID,
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

    /**+
     *
     *
     * @param other
     */
    ContactRecord(const ContactRecord& rec);

    /**
     *
     * @param other
     */
    ContactRecord(ContactRecord&& rec) noexcept;

    /**
     *
     * @param reader
     */
    explicit ContactRecord(util::BitReader& reader);

    /**
     *
     */
    ~ContactRecord() = default;

    /**
     *
     * @param rec
     * @return
     */
    ContactRecord& operator=(const ContactRecord& rec);

    /**
     * @brief
     * @param rec
     * @return
     */
    ContactRecord& operator=(ContactRecord&& rec) noexcept;

    /**
     *
     */
    void transposeCM();

    /**
     *
     * @return
     */
    uint8_t getSampleID() const;

    /**
     *
     * @return
     */
    const std::string& getSampleName() const;

    /**
     *
     * @return
     */
    uint32_t getBinSize() const;

    /**
     *
     * @return
     */
    uint8_t getChr1ID() const;

    /**
     *
     * @return
     */
    const std::string& getChr1Name() const;

    /**
     *
     * @return
     */
    uint64_t getChr1Length() const;

    /**
     *
     * @return
     */
    uint8_t getChr2ID() const;

    /**
     *
     * @return
     */
    const std::string& getChr2Name() const;

    /**
     *
     * @return
     */
    uint64_t getChr2Length() const;

    /**
     *
     * @return
     */
    uint64_t getNumCounts() const;

     /**
     *
     */
     uint8_t getNumNormCounts() const;

     /**
      *
      * @return
      */
     const std::vector<uint64_t>& getStartPos1() const;

     /**
      *
      * @return
      */
     const std::vector<uint64_t>& getEndPos1() const;

     /**
      *
      * @return
      */
     const std::vector<uint64_t>& getStartPos2() const;

     /**
      *
      * @return
      */
     const std::vector<uint64_t>& getEndPos2() const;

     /**
      *
      * @return
      */
     const std::vector<uint32_t>& getCounts() const;

     /**
      *
      */
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
