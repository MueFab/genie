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

#define MASK_ARR_BLEN 1
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
    std::optional<std::vector<bool>> mask_array;
    bool first_val;
    std::optional<std::vector<uint32_t>> rl_entries;

 public:
    /**
     * @brief Default constructor for SubcontactMatrixMaskPayload.
     *
     * This constructor initializes the object with default values.
     */
    SubcontactMatrixMaskPayload() = default;

    /**
     * @brief Constructor from move.
     *
     * This constructor moves the contents of another SubcontactMatrixMaskPayload object into this one.
     *
     * @param other The SubcontactMatrixMaskPayload object to move from.
     */
    SubcontactMatrixMaskPayload(SubcontactMatrixMaskPayload&&) = default;

    /**
     * @brief Constructor by reference.
     *
     * This constructor copies the contents of another SubcontactMatrixMaskPayload object into this one.
     *
     * @param other The SubcontactMatrixMaskPayload object to copy from.
     */
    SubcontactMatrixMaskPayload(const SubcontactMatrixMaskPayload&) = default;

    /**
     * @brief Constructor using operator=.
     *
     * This constructor assigns the contents of another SubcontactMatrixMaskPayload object to this one.
     *
     * @param other The SubcontactMatrixMaskPayload object to assign from.
     */
    SubcontactMatrixMaskPayload& operator=(
        const SubcontactMatrixMaskPayload&
    ) = default;

    /**
     * @brief Constructor that reads from a BitReader.
     *
     * This constructor reads the parameters from a BitReader.
     *
     * @param reader The BitReader to read from.
     * @param num_bin_entries The number of binary entries.
     */
    SubcontactMatrixMaskPayload(
        util::BitReader &reader,
        uint32_t num_bin_entries
    );

    /**
     * @brief Constructor that takes a mask array.
     *
     * This constructor takes a moveable mask array and initializes the object with it.
     *
     *  @param mask_array The moveable mask array.
     */
    SubcontactMatrixMaskPayload(
        BinVecDtype&& mask_array
    );

    /**
     * @brief Constructor that takes a transform ID, a boolean value, and a RLE-transformed mask array.
     *
     * This constructor takes a transform ID, a boolean value, and a run-length entries vector and initializes the object with them.
     *
     *  @param transform_ID The transform ID.
     *  @param first_val The boolean value.
     *  @param rl_entries The RLE-transformed mask array.
     */
    SubcontactMatrixMaskPayload(
        TransformID _transform_ID,
        bool _first_val,
        UIntVecDtype& _rl_entries
    );

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
    * @brief Checks if a mask array exists.
    *
    * Returns true if a mask array is present in the SubcontactMatrixMaskPayload object, otherwise false.
    *
    * @return True if a mask array exists, false otherwise.
    */
    bool anyMaskArray() const;

    /**
     * @brief Get the Mask Array.
     *
     * This function returns the Mask Array.
     *
     *  @return The Mask Array.
     */
    const std::vector<bool>& getMaskArray() const;

    /**
     *  @brief Get the First Value.
     *
     *  This function returns the First Value.
     *
     *  @return The First Value.
     */
    bool getFirstVal() const;

    /**
     * @brief Checks if run-length encoded entries exist.
     *
     * Returns true if run-length encoded entries are present in the SubcontactMatrixMaskPayload object, false otherwise.
     *
     * @return True if run-length encoded entries exist, false otherwise.
     */
    bool anyRLEntries() const;

    /**
     *  @brief Get the Run-Length Entries.
     *
     *  This function returns the Run-Length Entries.
     *
     *  @return The Run-Length Entries.
     */
    const std::vector<uint32_t>& getRLEntries() const;

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
     *  @param opt_array The new Mask Array.
     */
    void setMaskArray(const std::optional<BinVecDtype>& opt_array);

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
    void setRLEntries(
        TransformID _transform_ID,
        bool _first_val,
        const std::optional<UIntVecDtype>& _rl_entries
    );

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
