/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_DATASET_H_
#define SRC_GENIE_CORE_META_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "genie/core/meta/access-unit.h"
#include "genie/core/meta/blockheader.h"
#include "genie/core/meta/blockheader/enabled.h"
#include "genie/core/meta/dataset-group.h"
#include "genie/core/meta/descriptor-stream.h"
#include "genie/core/meta/label.h"
#include "genie/core/meta/reference.h"
#include "genie/util/make-unique.h"
#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::meta {

/**
 * @brief Metadata collection for a whole dataset
 */
class Dataset {
 private:
    std::optional<DatasetGroup> dataset_group;         //!< @brief Optional information about dataset group
    std::optional<Reference> reference;                //!< @brief Optional information about reference
    std::vector<Label> label_list;                     //!< @brief List of labels
    uint32_t version;                                  //!< @brief Current iteration of dataset
    std::unique_ptr<BlockHeader> headerCfg;            //!< @brief Configuration of block headers
    std::string DT_metadata_value;                     //!< @brief MPEG-G part 3 metadata
    std::string DT_protection_value;                   //!< @brief MPEG-G part 3 protection data
    std::vector<AccessUnit> access_units;              //!< @brief List of AU meta information
    std::vector<DescriptorStream> descriptor_streams;  //!< @brief List DS meta information

 public:
    /**
     * @brief
     * @param meta
     */
    void setMetadata(std::string meta);

    /**
     * @brief
     * @param prot
     */
    void setProtection(std::string prot);

    /**
     * @brief Construct from raw information
     * @param _version Current iteration
     * @param _headerCfg Block header mode
     * @param meta MPEG-G part 3 metadata
     * @param protection MPEG-G part 3 protection data
     */
    Dataset(uint32_t _version, std::unique_ptr<BlockHeader> _headerCfg, std::string meta, std::string protection);

    /**
     * @brief
     */
    Dataset();

    /**
     * @brief
     * @param _hdr
     */
    void setHeader(std::unique_ptr<BlockHeader> _hdr);

    /**
     * @brief Construct from json
     * @param json Json representation
     */
    explicit Dataset(const nlohmann::json& json);

    /**
     * @brief Convert to json
     * @return Json representation
     */
    [[nodiscard]] nlohmann::json toJson() const;

    /**
     * @brief Return dataset group information, if any
     * @return DatasetGroup information
     */
    [[nodiscard]] const std::optional<DatasetGroup>& getDataGroup() const;

    /**
     * @brief
     * @return
     */
    std::optional<DatasetGroup>& getDataGroup();

    /**
     * @brief Return reference information, if any
     * @return Reference information
     */
    [[nodiscard]] const std::optional<Reference>& getReference() const;

    /**
     * @brief
     * @return
     */
    std::optional<Reference>& getReference();

    /**
     * @brief Return list of labels
     * @return List of labels
     */
    [[nodiscard]] const std::vector<Label>& getLabels() const;

    /**
     * @brief
     * @return
     */
    std::vector<Label>& getLabels();

    /**
     * @brief Return current version iteration
     * @return Version iteration
     */
    [[nodiscard]] uint32_t getVersion() const;

    /**
     * @brief Return block header config
     * @return Block header config
     */
    [[nodiscard]] const BlockHeader& getHeader() const;

    /**
     * @brief Return MPEG-G Part 3 meta information
     * @return MPEG-G Part 3 meta information
     */
    [[nodiscard]] const std::string& getInformation() const;

    /**
     * @brief
     * @return
     */
    std::string& getInformation();

    /**
     * @brief Return MPEG-G Part 3 protection information
     * @return MPEG-G Part 3 protection information
     */
    [[nodiscard]] const std::string& getProtection() const;

    /**
     * @brief
     * @return
     */
    std::string& getProtection();

    /**
     * @brief Return list of access unit meta information blocks
     * @return AU meta information blocks
     */
    [[nodiscard]] const std::vector<AccessUnit>& getAUs() const;

    /**
     * @brief
     * @return
     */
    std::vector<AccessUnit>& getAUs();

    /**
     * @brief Return list of descriptor stream meta information blocks
     * @return Descriptor stream blocks
     */
    [[nodiscard]] const std::vector<DescriptorStream>& getDSs() const;

    /**
     * @brief
     * @return
     */
    std::vector<DescriptorStream>& getDSs();

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

}  // namespace genie::core::meta

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
