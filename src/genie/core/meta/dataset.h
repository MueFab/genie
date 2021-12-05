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
 * @brief
 */
class Dataset {
 private:
    boost::optional<DatasetGroup> dataset_group;       //!< @brief
    boost::optional<Reference> reference;              //!< @brief
    std::vector<Label> label_list;                     //!< @brief
    uint32_t version;                                  //!< @brief
    std::unique_ptr<BlockHeader> headerCfg;            //!< @brief
    std::string DT_metadata_value;                     //!< @brief
    std::string DT_protection_value;                   //!< @brief
    std::vector<AccessUnit> access_units;              //!< @brief
    std::vector<DescriptorStream> descriptor_streams;  //!< @brief

 public:
    /**
     * @brief
     * @param _version
     * @param _headerCfg
     * @param meta
     * @param protection
     */
    Dataset(uint32_t _version, std::unique_ptr<BlockHeader> _headerCfg, std::string meta, std::string protection);

    /**
     * @brief
     * @param json
     */
    explicit Dataset(const nlohmann::json& json);

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const;

    /**
     * @brief
     * @return
     */
    const boost::optional<DatasetGroup>& getDataGroup() const;

    /**
     * @brief
     * @return
     */
    const boost::optional<Reference>& getReference() const;

    /**
     * @brief
     * @return
     */
    const std::vector<Label>& getLabels() const;

    /**
     * @brief
     * @return
     */
    uint32_t getVersion() const;

    /**
     * @brief
     * @return
     */
    const BlockHeader& getHeader() const;

    /**
     * @brief
     * @return
     */
    const std::string& getInformation() const;

    /**
     * @brief
     * @return
     */
    const std::string& getProtection() const;

    /**
     * @brief
     * @return
     */
    const std::vector<AccessUnit>& getAUs() const;

    /**
     * @brief
     * @return
     */
    const std::vector<DescriptorStream>& getDSs() const;

    /**
     * @brief
     * @param dg
     */
    void setDataGroup(DatasetGroup dg);

    /**
     * @brief
     * @param ref
     */
    void setReference(Reference ref);

    /**
     * @brief
     * @param l
     */
    void addLabel(Label l);

    /**
     * @brief
     * @param au
     */
    void addAccessUnit(AccessUnit au);

    /**
     * @brief
     * @param ds
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
