/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_META_REFERENCE_H_
#define SRC_GENIE_CORE_META_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/core/meta/refbase.h"
#include "genie/core/meta/sequence.h"
#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {

/**
 * @brief
 */
class Reference {
 private:
    std::string reference_name;            //!< @brief
    uint32_t reference_major_version;      //!< @brief
    uint32_t reference_minor_version;      //!< @brief
    uint32_t reference_patch_version;      //!< @brief
    std::vector<Sequence> seqs;            //!< @brief
    std::unique_ptr<RefBase> ref;          //!< @brief
    std::string reference_metadata_value;  //!< @brief

 public:
    /**
     * @brief
     * @param name
     * @param major
     * @param minor
     * @param patch
     * @param base
     * @param seq1
     * @param metadata
     */
    Reference(std::string name, uint32_t major, uint32_t minor, uint32_t patch, std::unique_ptr<RefBase> base,
              Sequence seq1, std::string metadata);

    /**
     * @brief
     * @param json
     */
    explicit Reference(const nlohmann::json& json);

    /**
     * @brief
     * @return
     */
    nlohmann::json toJson() const;

    /**
     * @brief
     * @return
     */
    const std::string& getName() const;

    /**
     * @brief
     * @return
     */
    uint32_t getMajorVersion() const;

    /**
     * @brief
     * @return
     */
    uint32_t getMinorVersion() const;

    /**
     * @brief
     * @return
     */
    uint32_t getPatchVersion() const;

    /**
     * @brief
     * @return
     */
    const std::vector<Sequence>& getSequences() const;

    /**
     * @brief
     * @return
     */
    const RefBase& getBase() const;

    /**
     * @brief
     * @return
     */
    const std::string& getInformation() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
