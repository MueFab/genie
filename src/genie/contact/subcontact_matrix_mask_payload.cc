/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
 */

#include <ostream>
#include "genie/util/runtime-exception.h"
#include "subcontact_matrix_mask_payload.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixMaskPayload::operator==(const SubcontactMatrixMaskPayload& other) {
    return transform_ID == other.transform_ID &&
           mask_array == other.mask_array &&
           first_val == other.first_val &&
           rl_entries == other.rl_entries;
}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixMaskPayload::SubcontactMatrixMaskPayload(
    util::BitReader& reader,
    uint32_t num_bin_entries
) noexcept {
    transform_ID = reader.read<TransformID>(TRANSFORM_ID_BLEN);

    if (transform_ID == TransformID::ID_0){
        BinVecDtype tmp_mask_array = xt::empty<bool>({num_bin_entries}); // Not part of the spec
        for (auto i = 0u; i<num_bin_entries; i++){
            tmp_mask_array[i] = reader.read<bool>(1);
        }
        setMaskArray(tmp_mask_array);

    } else {
        first_val = reader.read<bool>(1);

        auto num_rl_entries = reader.read<uint32_t>();
        UIntVecDtype tmp_rl_entries = xt::empty<uint32_t>({num_rl_entries}); // Not part of the spec

        if (transform_ID == TransformID::ID_1){
            for (auto i = 0u; i<num_rl_entries; i++){
                tmp_rl_entries[i] = reader.read<uint8_t>();
            }
        } else if (transform_ID == TransformID::ID_2){
            for (auto i = 0u; i<num_rl_entries; i++){
                tmp_rl_entries[i] = reader.read<uint16_t>();
            }
        } else if (transform_ID == TransformID::ID_3){
            for (auto i = 0u; i<num_rl_entries; i++){
                tmp_rl_entries[i] = reader.read<uint32_t>();
            }
        } else {
            UTILS_DIE("This should never be reached!");
        }

        setRLEntries(tmp_rl_entries);
    }

    reader.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

TransformID SubcontactMatrixMaskPayload::getTransformID() const { return transform_ID; }

// ---------------------------------------------------------------------------------------------------------------------

std::optional<BinVecDtype> SubcontactMatrixMaskPayload::getMaskArray() const { return mask_array; }

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixMaskPayload::getFirstVal() const { return first_val; }

// ---------------------------------------------------------------------------------------------------------------------

std::optional<UIntVecDtype> SubcontactMatrixMaskPayload::getRLEntries() const { return rl_entries; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixMaskPayload::setTransformID(TransformID id) { transform_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixMaskPayload::setMaskArray(const std::optional<BinVecDtype>& array) { mask_array = array; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixMaskPayload::setFirstVal(bool val) { first_val = val; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixMaskPayload::setRLEntries(const std::optional<UIntVecDtype>& _rl_entries) { rl_entries = _rl_entries; }

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixMaskPayload::getSize() const{
    size_t size_bits = TRANSFORM_ID_BLEN; // transform_ID
    if (transform_ID == TransformID::ID_0){
        size_bits += mask_array->shape(0);
    } else {
        size_bits += FIRST_VAL_BLEN;
        size_bits += NUM_RL_ENTRIES_BLEN;
        size_bits += rl_entries->shape(0) * (4u << static_cast<uint8_t>(transform_ID));
    };

    return static_cast<size_t>(ceil(static_cast<double>(size_bits) / 8));
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixMaskPayload::write(util::BitWriter &writer) const{
    // Write everything on-memory before dumping it to the writer
    auto payload = std::stringstream();
    std::ostream& _writer = payload;
    auto onmem_writer = genie::util::BitWriter(&_writer);

    onmem_writer.write(static_cast<uint64_t>(transform_ID), TRANSFORM_ID_BLEN);
    if (transform_ID == TransformID::ID_0){
        auto num_bin_entries = mask_array->shape(0);
        for (auto i = 0u; i < num_bin_entries; i++){
            onmem_writer.write(mask_array->at(i), 1);
        }
    } else {
        onmem_writer.write(first_val, 1);
        auto num_rl_entries = rl_entries->shape(0);
        onmem_writer.write(num_rl_entries, NUM_RL_ENTRIES_BLEN);
        for (auto i = 0u; i < num_rl_entries; i++){
            auto nbits = static_cast<uint8_t>(4u << static_cast<uint8_t>(transform_ID));
            auto val = static_cast<uint64_t>(rl_entries->at(i));
            onmem_writer.write(val, nbits);
        }
    }
    // Align to byte
    onmem_writer.flush();

    auto payload_str = payload.str();
    for (auto& v: payload_str){
        writer.write(static_cast<uint64_t>(v), 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

} // contact
} // genie

// ---------------------------------------------------------------------------------------------------------------------