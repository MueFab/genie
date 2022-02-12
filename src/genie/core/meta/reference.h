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
 * @brief Reference metadata
 */
class Reference {
 private:
    std::string reference_name;            //!< @brief Name of the full reference
    uint32_t reference_major_version;      //!< @brief Major version level
    uint32_t reference_minor_version;      //!< @brief Minor version level
    uint32_t reference_patch_version;      //!< @brief Patch level
    std::vector<Sequence> seqs;            //!< @brief List of reference sequences
    std::unique_ptr<RefBase> ref;          //!< @brief Reference type specific information
    std::string reference_metadata_value;  //!< @brief MPEG-G part 3 meta information

 public:
    /**
     * @brief
     * @param _ref
     */
    Reference(const Reference& _ref);

    /**
     * @brief
     * @param _ref
     */
    Reference(Reference&& _ref) noexcept;

    /**
     * @brief
     * @param _ref
     * @return
     */
    Reference& operator=(const Reference& _ref);

    /**
     * @brief
     * @param _ref
     * @return
     */
    Reference& operator=(Reference&& _ref) noexcept;

    /**
     * @brief Construct from raw data
     * @param name Reference name
     * @param major Major version level
     * @param minor Minor version level
     * @param patch Patch version level
     * @param base Reference type specific information
     * @param metadata MPEG-G part 3 meta information
     */
    Reference(std::string name, uint32_t major, uint32_t minor, uint32_t patch, std::unique_ptr<RefBase> base,
              std::string metadata);

    /**
     * @brief Construct from json
     * @param json Json representation
     */
    explicit Reference(const nlohmann::json& json);

    /**
     * @brief Convert to json
     * @return Json representation
     */
    nlohmann::json toJson() const;

    /**
     * @brief Return sequence name
     * @return Sequence name
     */
    const std::string& getName() const;

    /**
     * @brief
     * @return
     */
    std::string& getName();

    /**
     * @brief Return major version level
     * @return Major version
     */
    uint32_t getMajorVersion() const;

    /**
     * @brief Return minor version level
     * @return Minor version level
     */
    uint32_t getMinorVersion() const;

    /**
     * @brief Return patch version level
     * @return Patch version level
     */
    uint32_t getPatchVersion() const;

    /**
     * @brief Return list of reference sequences
     * @return Information about all reference sequences
     */
    const std::vector<Sequence>& getSequences() const;

    /**
     * @brief
     * @return
     */
    std::vector<Sequence>& getSequences();

    /**
     * @brief Add a new reference sequence
     * @param s Sequence to add
     */
    void addSequence(Sequence s);

    /**
     * @brief Get reference type specific information
     * @return Reference type specific information
     */
    const RefBase& getBase() const;

    /**
     * @brief Get reference type specific information
     * @return Reference type specific information
     */
    std::unique_ptr<RefBase> moveBase();

    /**
     * @brief Return MPEG-G part 3 meta information
     * @return MPEG-G part 3 meta information
     */
    const std::string& getInformation() const;

    /**
     * @brief
     * @return
     */
    std::string& getInformation();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_META_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
