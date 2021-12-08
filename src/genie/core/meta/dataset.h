/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_DATASET_H_
#define SRC_GENIE_CORE_META_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "boost/optional/optional.hpp"
#include "genie/core/meta/access-unit.h"
#include "genie/core/meta/blockheader.h"
#include "genie/core/meta/dataset-group.h"
#include "genie/core/meta/descriptor-stream.h"
#include "genie/core/meta/label.h"
#include "genie/core/meta/reference.h"
#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {

/**
 * @brief Metadata collection for a whole dataset
 */
class Dataset {
 private:
    boost::optional<DatasetGroup> dataset_group;       //!< @brief Optional information about dataset group
    boost::optional<Reference> reference;              //!< @brief Optional information about reference
    std::vector<Label> label_list;                     //!< @brief List of labels
    uint32_t version;                                  //!< @brief Current iteration of dataset
    std::unique_ptr<BlockHeader> headerCfg;            //!< @brief Configuration of block headers
    std::string DT_metadata_value;                     //!< @brief MPEG-G part 3 metadata
    std::string DT_protection_value;                   //!< @brief MPEG-G part 3 protection data
    std::vector<AccessUnit> access_units;              //!< @brief List of AU meta information
    std::vector<DescriptorStream> descriptor_streams;  //!< @brief List DS meta information

 public:
    /**
     * @brief Construct from raw information
     * @param _version Current iteration
     * @param _headerCfg Block header mode
     * @param meta MPEG-G part 3 metadata
     * @param protection MPEG-G part 3 protection data
     */
    Dataset(uint32_t _version, std::unique_ptr<BlockHeader> _headerCfg, std::string meta, std::string protection);

    /**
     * @brief Construct from json
     * @param json Json representation
     */
    explicit Dataset(const nlohmann::json& json);

    /**
     * @brief Convert to json
     * @return Json representation
     */
    nlohmann::json toJson() const;

    /**
     * @brief Return dataset group information, if any
     * @return DatasetGroup information
     */
    const boost::optional<DatasetGroup>& getDataGroup() const;

    /**
     * @brief Return reference information, if any
     * @return Reference information
     */
    const boost::optional<Reference>& getReference() const;

    /**
     * @brief Return list of labels
     * @return List of labels
     */
    const std::vector<Label>& getLabels() const;

    /**
     * @brief Return current version iteration
     * @return Version iteration
     */
    uint32_t getVersion() const;

    /**
     * @brief Return block header config
     * @return Block header config
     */
    const BlockHeader& getHeader() const;

    /**
     * @brief Return MPEG-G Part 3 meta information
     * @return MPEG-G Part 3 meta information
     */
    const std::string& getInformation() const;

    /**
     * @brief Return MPEG-G Part 3 protection information
     * @return MPEG-G Part 3 protection information
     */
    const std::string& getProtection() const;

    /**
     * @brief Return list of access unit meta information blocks
     * @return AU meta information blocks
     */
    const std::vector<AccessUnit>& getAUs() const;

    /**
     * @brief Return list of descriptor stream meta information blocks
     * @return Descriptor stream blocks
     */
    const std::vector<DescriptorStream>& getDSs() const;

    /**
     * @brief Update dataset group information
     * @param dg New dataset group information
     */
    void setDataGroup(DatasetGroup dg);

    /**
     * @brief Update reference meta information
     * @param ref New reference meta information
     */
    void setReference(Reference ref);

    /**
     * @brief Add a new label to the dataset
     * @param l New label
     */
    void addLabel(Label l);

    /**
     * @brief Add access unit meta data block
     * @param au Access unit meta data block
     */
    void addAccessUnit(AccessUnit au);

    /**
     * @brief Add descriptor stream meta data block
     * @param ds New data set meta data block
     */
    void addDescriptorStream(DescriptorStream ds);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
