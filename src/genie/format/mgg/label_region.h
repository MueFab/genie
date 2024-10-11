/**
 * @file
 * @brief Defines the LabelRegion class for representing labeled regions in MPEG-G files.
 * @details This file contains the implementation of the `LabelRegion` class, which is used to define and manage
 *          labeled regions within a genomic sequence. Each region is characterized by its sequence ID, start and end
 *          positions, and associated class IDs, which are used to classify and annotate various genomic regions.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_LABEL_REGION_H_
#define SRC_GENIE_FORMAT_MGG_LABEL_REGION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/core/meta/region.h"
#include "genie/core/record/class-type.h"
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief Class representing a labeled region in an MPEG-G dataset.
 * @details The LabelRegion class encapsulates metadata associated with a specific region of a genomic sequence.
 *          It includes details such as the sequence ID, start and end positions of the region, and a list of class IDs
 *          that categorize the region.
 */
class LabelRegion {
 private:
    uint16_t seq_ID;  //!< @brief Identifier of the sequence this region belongs to.
    std::vector<genie::core::record::ClassType> class_IDs;  //!< @brief List of class IDs associated with the region.
    uint64_t start_pos;                                     //!< @brief Start position of the region (0-based).
    uint64_t end_pos;                                       //!< @brief End position of the region (inclusive).

 public:
    /**
     * @brief Convert the LabelRegion object to a genie::core::meta::Region instance.
     * @return A Region object containing the sequence ID, start and end positions, and class types.
     */
    genie::core::meta::Region decapsulate();

    /**
     * @brief Equality operator to compare two LabelRegion objects.
     * @param other The other LabelRegion object to compare.
     * @return True if both regions are equivalent.
     */
    bool operator==(const LabelRegion& other) const;

    /**
     * @brief Construct a new LabelRegion object.
     * @param _seq_ID Sequence ID of the region.
     * @param _start_pos Start position of the region.
     * @param end_pos End position of the region.
     */
    LabelRegion(uint16_t _seq_ID, uint64_t _start_pos, uint64_t end_pos);

    /**
     * @brief Construct a new LabelRegion object by reading from a BitReader.
     * @param reader The BitReader to extract the region information from.
     */
    explicit LabelRegion(util::BitReader& reader);

    /**
     * @brief Add a class ID to the list of associated class IDs for this region.
     * @param _class_ID Class ID to be added.
     */
    void addClassID(genie::core::record::ClassType _class_ID);

    /**
     * @brief Retrieve the sequence ID of the region.
     * @return Sequence ID.
     */
    [[nodiscard]] uint16_t getSeqID() const;

    /**
     * @brief Retrieve the list of associated class IDs for this region.
     * @return List of class IDs.
     */
    [[nodiscard]] const std::vector<genie::core::record::ClassType>& getClassIDs() const;

    /**
     * @brief Retrieve the start position of the region.
     * @return Start position.
     */
    [[nodiscard]] uint64_t getStartPos() const;

    /**
     * @brief Retrieve the end position of the region.
     * @return End position.
     */
    [[nodiscard]] uint64_t getEndPos() const;

    /**
     * @brief Write the LabelRegion data to a BitWriter for serialization.
     * @param bit_writer The BitWriter to serialize the region data to.
     */
    void write(genie::util::BitWriter& bit_writer) const;

    /**
     * @brief Calculate the size of the LabelRegion data when serialized.
     * @return The size in bits.
     */
    [[nodiscard]] uint64_t sizeInBits() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_LABEL_REGION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
