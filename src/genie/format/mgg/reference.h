/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/meta/reference.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/format/mgg/reference/location.h"
#include "genie/format/mgg/reference/sequence.h"
#include "genie/format/mgg/reference/version.h"
#include "genie/util/bitreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

/**
 * @brief
 */
class Reference : public GenInfo {
 public:
    enum class ReferenceType : uint8_t { MPEGG_REF = 0, RAW_REF = 1, FASTA_REF = 2 };  //!< @brief

 private:
    uint8_t dataset_group_ID;        //!< @brief
    uint8_t reference_ID;            //!< @brief
    std::string reference_name;      //!< @brief
    reference::Version ref_version;  //!< @brief

    std::vector<reference::Sequence> sequences;  //!< @brief seq_count is length of vector

    std::unique_ptr<reference::Location> reference_location;  //!< @brief

    genie::core::MPEGMinorVersion version;  //!< @brief

 public:
    /**
     * @brief
     * @param groupID
     */
    void patchID(uint8_t groupID);

    /**
     * @brief
     * @param _old
     * @param _new
     */
    void patchRefID(uint8_t _old, uint8_t _new);

    /**
     * @brief
     */
    genie::core::meta::Reference decapsulate(std::string meta);

    /**
     * @brief
     * @param _dataset_group_id
     * @param _reference_ID
     * @param ref
     * @param _version
     */
    Reference(uint8_t _dataset_group_id, uint8_t _reference_ID, genie::core::meta::Reference ref,
              genie::core::MPEGMinorVersion _version);

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     * @param reader
     * @param _version
     */
    explicit Reference(util::BitReader& reader, genie::core::MPEGMinorVersion _version);

    /**
     * @brief
     * @param group_id
     * @param ref_id
     * @param ref_name
     * @param _ref_version
     * @param location
     * @param _version
     */
    Reference(uint8_t group_id, uint8_t ref_id, std::string ref_name, reference::Version _ref_version,
              std::unique_ptr<reference::Location> location, genie::core::MPEGMinorVersion _version);

    /**
     * @brief
     * @return
     */
    uint8_t getDatasetGroupID() const;

    /**
     * @brief
     * @return
     */
    uint8_t getReferenceID() const;

    /**
     * @brief
     * @return
     */
    const std::string& getReferenceName() const;

    /**
     * @brief
     * @return
     */
    const reference::Version& getRefVersion() const;

    /**
     * @brief
     * @return
     */
    const std::vector<reference::Sequence>& getSequences() const;

    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;

    /**
     * @brief
     * @param seq
     * @param checksum
     */
    void addSequence(reference::Sequence seq, std::string checksum);

    /**
     * @brief
     * @return
     */
    const reference::Location& getLocation() const;

    /**
     * @brief
     * @param writer
     */
    void box_write(genie::util::BitWriter& writer) const override;

    /**
     * @brief
     * @param output
     * @param depth
     * @param max_depth
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
