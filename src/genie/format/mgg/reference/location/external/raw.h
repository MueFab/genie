/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_RAW_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_RAW_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/format/mgg/reference/location/external.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::reference::location::external {

/**
 * @brief
 */
class Raw : public External {
 private:
    std::vector<std::string> seq_checksums;  //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    std::unique_ptr<genie::core::meta::RefBase> decapsulate() override;

    /**
     * @brief
     * @param _reserved
     * @param _uri
     * @param algo
     */
    Raw(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo);

    /**
     * @brief
     * @param reader
     * @param seq_count
     */
    Raw(genie::util::BitReader& reader, size_t seq_count);

    /**
     * @brief
     * @param reader
     * @param _reserved
     * @param _uri
     * @param algo
     * @param seq_count
     */
    Raw(genie::util::BitReader& reader, uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo, size_t seq_count);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<std::string>& getSeqChecksums() const;

    /**
     * @brief
     * @param checksum
     */
    void addSeqChecksum(std::string checksum);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) override;

    /**
     * @brief
     * @param checksum
     */
    void addChecksum(std::string checksum) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference::location::external

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_RAW_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
