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

SubcontactMatrixMaskPayload::SubcontactMatrixMaskPayload(
    util::BitReader& reader,
    uint32_t num_bin_entries
) noexcept {
    transform_ID = reader.read<TransformID>(TRANSFORM_ID_BLEN);

    if (transform_ID == TransformID::ID_0){
        BinVecDtype tmp_mask_array = xt::empty<bool>({num_bin_entries}); // Not part of the spec
        for (auto i = 0u; i<num_bin_entries; i++){
            tmp_mask_array[i] = reader.read<bool>(MASK_ARR_BLEN);
        }
        setMaskArray(tmp_mask_array);
        first_val = mask_array->at(0);

    } else {
        first_val = reader.read<bool>(FIRST_VAL_BLEN);

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

        setRLEntries(
            transform_ID,
            first_val,
            tmp_rl_entries
        );
    }

    reader.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixMaskPayload::SubcontactMatrixMaskPayload(
    BinVecDtype&& _mask_array
) noexcept
    : transform_ID(TransformID::ID_0),
      mask_array(),
      first_val(),
      rl_entries()
{
    setMaskArray(_mask_array);
}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixMaskPayload::SubcontactMatrixMaskPayload(
    TransformID _transform_ID,
    bool _first_val,
    UIntVecDtype& _rl_entries
) noexcept
    : transform_ID(_transform_ID),
      mask_array(),
      first_val(_first_val)
{
    UTILS_DIE_IF(_transform_ID == TransformID::ID_0, "Invalid transform_ID!");
    rl_entries = std::vector<uint32_t>(_rl_entries.begin(), _rl_entries.end());
}

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixMaskPayload::operator==(const SubcontactMatrixMaskPayload& other) {
    return transform_ID == other.transform_ID &&
           mask_array == other.mask_array &&
           first_val == other.first_val &&
           rl_entries == other.rl_entries;
}

// ---------------------------------------------------------------------------------------------------------------------

TransformID SubcontactMatrixMaskPayload::getTransformID() const { return transform_ID; }

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixMaskPayload::anyMaskArray() const{ return mask_array.has_value();}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<bool>& SubcontactMatrixMaskPayload::getMaskArray() const{
    UTILS_DIE_IF(
        !anyMaskArray(),
        "mask_array is not set!"
    );
    return *mask_array;
}

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixMaskPayload::getFirstVal() const { return first_val; }

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixMaskPayload::anyRLEntries() const{ return rl_entries.has_value();}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint32_t>& SubcontactMatrixMaskPayload::getRLEntries() const {
    UTILS_DIE_IF(
        !anyRLEntries(),
        "mask_array is not set!"
    );
    return *rl_entries;
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixMaskPayload::setTransformID(TransformID id) { transform_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixMaskPayload::setMaskArray(
    const std::optional<BinVecDtype>& opt_array
) {
    transform_ID = TransformID::ID_0;
    if (rl_entries.has_value()){
        rl_entries = {};
    }

    if (opt_array.has_value()){
        UTILS_DIE_IF(opt_array->shape(0) == 0, "Invalid opt_array size!");

        auto& array = opt_array.value();
        auto std_array = std::vector<bool>(array.begin(), array.end());
        first_val = std_array[0];
        mask_array = std::move(std_array);
    } else {
        first_val = false;
        mask_array = {};
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixMaskPayload::setFirstVal(bool val) {
    UTILS_DIE_IF(
        transform_ID == TransformID::ID_0,
        "first_val can only be set manually for transform_ID 1-3!"
    );

    first_val = val;
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixMaskPayload::setRLEntries(
    TransformID _transform_ID,
    bool _first_val,
    const std::optional<UIntVecDtype>& _rl_entries
) {
    UTILS_DIE_IF(_transform_ID == TransformID::ID_0, "transform_ID 0 is not allowed here!");
    transform_ID = _transform_ID;

    if (mask_array.has_value()){
        mask_array = {};
    }

    if (_rl_entries.has_value()){
        UTILS_DIE_IF(_rl_entries->shape(0) == 0, "Invalid opt_array size!");

        auto& array = _rl_entries.value();
        auto std_array = std::vector<uint32_t>(array.begin(), array.end());
        first_val = _first_val;
        rl_entries = std::move(std_array);
    } else {
        first_val = false;
        rl_entries = {};
    }
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixMaskPayload::getSize() const{
    size_t size_bits = TRANSFORM_ID_BLEN; // transform_ID
    if (transform_ID == TransformID::ID_0){
        UTILS_DIE_IF(!mask_array.has_value(), "mask_array is missing?");
        size_bits += mask_array.value().size();
    } else {
        UTILS_DIE_IF(!rl_entries.has_value(), "rl_entries is missing?");
        size_bits += FIRST_VAL_BLEN;
        size_bits += NUM_RL_ENTRIES_BLEN;
        size_bits += rl_entries.value().size() * (4u << static_cast<uint8_t>(transform_ID));
    };

    return static_cast<size_t>(ceil(static_cast<double>(size_bits) / 8));
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixMaskPayload::write(util::BitWriter &writer) const{
    UTILS_DIE_IF(
        !mask_array.has_value() && !rl_entries.has_value(),
        "Both mask_array and rl_entries are empty!"
    );

    // Write everything on-memory before dumping it to the writer
    auto payload = std::stringstream();
    std::ostream& _writer = payload;
    auto onmem_writer = genie::util::BitWriter(&_writer);

    onmem_writer.write(
        static_cast<uint64_t>(transform_ID),
        TRANSFORM_ID_BLEN
    );

    if (transform_ID == TransformID::ID_0){
        UTILS_DIE_IF(!mask_array.has_value(), "mask_array is missing?");
        auto num_bin_entries = mask_array->size();

        for (auto i = 0u; i < num_bin_entries; i++){
            auto _mask_array = mask_array.value();
            uint64_t val = mask_array->at(i);
            onmem_writer.write(val, 1);
        }
    } else {
        UTILS_DIE_IF(!rl_entries.has_value(), "rl_entries is missing?");
        onmem_writer.write(first_val, FIRST_VAL_BLEN);
        auto num_rl_entries = rl_entries->size();
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
        writer.writeBypassBE(v);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

} // contact
} // genie

// ---------------------------------------------------------------------------------------------------------------------