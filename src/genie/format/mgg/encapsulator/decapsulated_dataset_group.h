/**
 * @file
 * @brief Defines the `DecapsulatedDatasetGroup` class for handling the decapsulation of MPEG-G dataset groups.
 *
 * The `DecapsulatedDatasetGroup` class is part of the MPEG-G encapsulation/decapsulation library.
 * It provides methods to extract and manage dataset groups and related metadata from encapsulated
 * MPEG-G dataset files. This class is primarily used for processing and transforming MPEG-G
 * dataset groups into a format suitable for further processing or re-encapsulation.
 *
 * @copyright
 * This file is part of GENIE. See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_DECAPSULATED_DATASET_GROUP_H_
#define SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_DECAPSULATED_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <optional>
#include <utility>
#include "genie/format/mgb/mgb_file.h"
#include "genie/format/mgg/dataset_group.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::encapsulator {

/**
 * @class DecapsulatedDatasetGroup
 * @brief Class for handling decapsulation of MPEG-G dataset groups.
 *
 * The `DecapsulatedDatasetGroup` class is used to transform MPEG-G dataset groups from their encapsulated
 * format into decapsulated components, including metadata, reference sequences, and individual datasets.
 * This decapsulation allows easy access to the internal components for further processing or re-encapsulation.
 */
class DecapsulatedDatasetGroup {
 private:
    uint64_t id{};  //!< @brief Unique identifier for the decapsulated dataset group.

    // Optional metadata group information extracted during decapsulation.
    std::optional<genie::core::meta::DatasetGroup> meta_group;

    // Map storing metadata references associated with the dataset group.
    std::map<uint8_t, genie::core::meta::Reference> meta_references;

    // Map storing datasets, indexed by dataset ID, along with the associated metadata.
    std::map<uint64_t, std::pair<genie::format::mgb::MgbFile, genie::core::meta::Dataset>> data;

    /**
     * @brief Decapsulates the dataset group to extract high-level metadata.
     *
     * @param grp Pointer to the encapsulated `DatasetGroup`.
     * @return An optional `DatasetGroup` containing decapsulated metadata.
     */
    static std::optional<genie::core::meta::DatasetGroup> decapsulate_dataset_group(
        genie::format::mgg::DatasetGroup* grp);

    /**
     * @brief Decapsulates references from the dataset group.
     *
     * @param grp Pointer to the encapsulated `DatasetGroup`.
     * @return A map of reference IDs to decapsulated `Reference` objects.
     */
    static std::map<uint8_t, genie::core::meta::Reference> decapsulate_references(
        genie::format::mgg::DatasetGroup* grp);

    /**
     * @brief Decapsulates a single Access Unit (AU) from the encapsulated format.
     *
     * @param au The encapsulated `AccessUnit` to decapsulate.
     * @return A pair containing the decapsulated `AccessUnit` and optional metadata.
     */
    static std::pair<genie::format::mgb::AccessUnit, std::optional<genie::core::meta::AccessUnit>> decapsulate_AU(
        genie::format::mgg::AccessUnit& au);

    /**
     * @brief Decapsulates a dataset and its associated metadata.
     *
     * @param dt The encapsulated `Dataset` to decapsulate.
     * @param meta_group Optional reference to the associated `DatasetGroup` metadata.
     * @param meta_references Map of reference IDs to associated `Reference` metadata.
     * @param grp Pointer to the parent `DatasetGroup`.
     * @return A pair containing the decapsulated `MgbFile` and associated `Dataset` metadata.
     */
    static std::pair<genie::format::mgb::MgbFile, genie::core::meta::Dataset> decapsulate_dataset(
        genie::format::mgg::Dataset& dt, std::optional<genie::core::meta::DatasetGroup>& meta_group,
        std::map<uint8_t, genie::core::meta::Reference>& meta_references, genie::format::mgg::DatasetGroup* grp);

 public:
    /**
     * @brief Move constructor for the `DecapsulatedDatasetGroup` class.
     *
     * Constructs a `DecapsulatedDatasetGroup` by transferring ownership of internal data from another instance.
     *
     * @param other The `DecapsulatedDatasetGroup` to move from.
     */
    DecapsulatedDatasetGroup(DecapsulatedDatasetGroup&& other) noexcept;

    /**
     * @brief Constructs a `DecapsulatedDatasetGroup` from an encapsulated dataset group.
     *
     * This constructor decapsulates the provided `DatasetGroup` and extracts its components
     * into separate fields for easier manipulation.
     *
     * @param grp Pointer to the encapsulated `DatasetGroup` to decapsulate.
     */
    explicit DecapsulatedDatasetGroup(genie::format::mgg::DatasetGroup* grp);

    /**
     * @brief Retrieves the unique identifier for the dataset group.
     *
     * @return The unique ID of the dataset group.
     */
    [[nodiscard]] uint64_t getID() const;

    /**
     * @brief Checks if the dataset group has associated metadata.
     *
     * @return `true` if metadata is present, `false` otherwise.
     */
    [[nodiscard]] bool hasMetaGroup() const;

    /**
     * @brief Retrieves the associated metadata group for the dataset.
     *
     * @return Reference to the associated `DatasetGroup` metadata.
     * @throws `std::runtime_error` if the metadata is not present.
     */
    genie::core::meta::DatasetGroup& getMetaGroup();

    /**
     * @brief Retrieves the reference metadata associated with the dataset group.
     *
     * @return Map of reference IDs to associated `Reference` metadata.
     */
    std::map<uint8_t, genie::core::meta::Reference>& getMetaReferences();

    /**
     * @brief Retrieves the decapsulated datasets and their metadata.
     *
     * @return Map of dataset IDs to associated `MgbFile` and `Dataset` metadata pairs.
     */
    std::map<uint64_t, std::pair<genie::format::mgb::MgbFile, genie::core::meta::Dataset>>& getData();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::encapsulator

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_ENCAPSULATOR_DECAPSULATED_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
