/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CONTACT_SUBCONTACT_MATRIX_MASK_PAYLOAD_H
#define GENIE_CONTACT_SUBCONTACT_MATRIX_MASK_PAYLOAD_H

#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "consts.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

#define TRANSFORM_ID_BLEN 2
#define FIRST_VAL_BLEN 1
#define NUM_RL_ENTRIES_BLEN 32

// ---------------------------------------------------------------------------------------------------------------------

enum class TransformID : uint8_t {
    ID_0 = 0,
    ID_1 = 1,
    ID_2 = 2,
    ID_3 = 3,
};

// ---------------------------------------------------------------------------------------------------------------------

class SubcontactMatrixMaskPayload{
 private:
    TransformID transform_ID;
    std::optional<BinVecDtype> mask_array;
    bool first_val;
    std::optional<UIntVecDtype> rl_entries;

 public:
    // Default constructor
    SubcontactMatrixMaskPayload() = default;

    // Constructor from move
    SubcontactMatrixMaskPayload(SubcontactMatrixMaskPayload&&) = default;

    // Constructor by reference
    SubcontactMatrixMaskPayload(const SubcontactMatrixMaskPayload&) = default;

    // Constructor using operator=
    SubcontactMatrixMaskPayload& operator=(const SubcontactMatrixMaskPayload&) = default;

    SubcontactMatrixMaskPayload(
        util::BitReader &reader,
        uint32_t num_bin_entries
    ) noexcept ;

    /**
     * @brief Overloaded equality operator.
     *
     * This operator compares two SubcontactMatrixMaskPayload objects for equality.
     * It returns true if all the member variables of the two objects are equal, and false otherwise.
     *
     *  @param other The SubcontactMatrixMaskPayload object to compare with.
     *  @return True if the two objects are equal, false otherwise.
     */
    bool operator==(const SubcontactMatrixMaskPayload& other);

    /**
     * @brief Get the Transform ID.
     *
     * This function returns the Transform ID.
     *
     *  @return The Transform ID.
     */
    TransformID getTransformID() const;

    /**
     * @brief Get the Mask Array.
     *
     * This function returns the Mask Array.
     *
     *  @return The Mask Array.
     */
    std::optional<BinVecDtype> getMaskArray() const;

    /**
     *  @brief Get the First Value.
     *
     *  This function returns the First Value.
     *
     *  @return The First Value.
     */
    bool getFirstVal() const;

    /**
     *  @brief Get the Run-Length Entries.
     *
     *  This function returns the Run-Length Entries.
     *
     *  @return The Run-Length Entries.
     */
    std::optional<UIntVecDtype> getRLEntries() const;

    /**
     *  @brief Set the Transform ID.
     *
     *  This function sets the Transform ID.
     *
     *  @param id The new Transform ID.
     */
    void setTransformID(TransformID id);

    /**
     *  @brief Set the Mask Array.
     *
     *  This function sets the Mask Array.
     *
     *  @param array The new Mask Array.
     */
    void setMaskArray(const std::optional<BinVecDtype>& array);

    /**
     *  @brief Set the First Value.
     *
     *  This function sets the First Value.
     *
     *  @param val The new First Value.
     */
    void setFirstVal(bool val);

    /**
     *  @brief Set the Run-Length Entries.
     *
     *  This function sets the Run-Length Entries.
     *
     *  @param _rl_entries The new Run-Length Entries.
     */
    void setRLEntries(const std::optional<UIntVecDtype>& _rl_entries);

    /**
     * @brief Gets the size of this structure.
     *
     * This function returns the size of this structure.
     *
     * @return The size of the payload.
     */
    size_t getSize() const;

    /**
     * @brief Writes the object to a writer.
     *
     * This function writes the object to a writer.
     *
     * @param writer The writer to write to.
     */
    void write(util::BitWriter &writer) const;


};


// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_SUBCONTACT_MATRIX_MASK_PAYLOAD_H
