/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include "genie/format/mgg/reference/location.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace reference {
namespace location {

/**
 * @brief
 */
class External : public Location {
 public:
    /**
     * @brief
     */
    enum class ChecksumAlgorithm : uint8_t { MD5 = 0, SHA256 = 1 };

    /**
     * @brief
     */
    static const size_t checksum_sizes[];

    /**
     * @brief
     */
    enum class RefType : uint8_t { MPEGG_REF = 0, RAW_REF = 1, FASTA_REF = 2 };

 private:
    std::string uri;                  //!< @brief
    ChecksumAlgorithm checksum_algo;  //!< @brief
    RefType reference_type;           //!< @brief

 public:
    /**
     * @brief
     * @param _reserved
     * @param _uri
     * @param algo
     * @param type
     */
    External(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo, RefType type);

    /**
     * @brief
     * @param reader
     */
    explicit External(genie::util::BitReader& reader);

    /**
     * @brief
     * @param reader
     * @param _reserved
     */
    External(genie::util::BitReader& reader, uint8_t _reserved);

    /**
     * @brief
     * @return
     */
    const std::string& getURI() const;

    /**
     * @brief
     * @return
     */
    std::string& getURI();

    /**
     * @brief
     * @return
     */
    ChecksumAlgorithm getChecksumAlgorithm() const;

    /**
     * @brief
     * @return
     */
    RefType getReferenceType() const;

    /**
     * @brief
     * @param reader
     * @param _reserved
     * @param seq_count
     * @param _version
     * @return
     */
    static std::unique_ptr<Location> factory(genie::util::BitReader& reader, uint8_t _reserved, size_t seq_count,
                                             genie::core::MPEGMinorVersion _version);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) override;

    /**
     * @brief
     * @param checksum
     */
    virtual void addChecksum(std::string checksum) = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace location
}  // namespace reference
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
