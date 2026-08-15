/**
* @file
* @copyright This file is part of GENIE. See LICENSE and/or
* https://github.com/mitogen/genie for more details.
 */

#include "genie/contact/subcontact_matrix_mask_payload.h"
#include <ostream>
#include <sstream>
#include <utility>
#include <vector>
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::contact {

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixMaskPayload::SubcontactMatrixMaskPayload(
    util::BitReader& reader,
    uint32_t num_bin_entries
) {
  transform_ID_ = reader.Read<TransformID>(TRANSFORM_ID_BLEN);

    if (transform_ID_ == TransformID::ID_0) {
#if defined(GENIE_CONTACT_BACKEND_XTENSOR)
        BinVecDtype tmp_mask_array = xt::empty<bool>({num_bin_entries});  // Not part of the spec
        for (auto idx_i = 0u; idx_i < num_bin_entries; idx_i++) {
            tmp_mask_array[idx_i] = reader.Read<bool>(MASK_ARR_BLEN);
        }
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
        BinVecDtype tmp_mask_array(num_bin_entries);
        for (auto idx_i = 0u; idx_i < num_bin_entries; idx_i++) {
            tmp_mask_array(idx_i) = reader.Read<bool>(MASK_ARR_BLEN);
        }
#else
        BinVecDtype tmp_mask_array(num_bin_entries);
        for (auto idx_i = 0u; idx_i < num_bin_entries; idx_i++) {
            tmp_mask_array[idx_i] = reader.Read<bool>(MASK_ARR_BLEN);
        }
#endif
        SetMaskArray(tmp_mask_array);
        first_val_ = mask_array_->at(0);

    } else {
        first_val_ = reader.Read<bool>(FIRST_VAL_BLEN);
        auto num_rl_entries = reader.Read<uint32_t>();
#if defined(GENIE_CONTACT_BACKEND_XTENSOR)
        UIntVecDtype tmp_rl_entries = xt::empty<uint32_t>({num_rl_entries});  // Not part of the spec
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
        UIntVecDtype tmp_rl_entries(num_rl_entries);
#else
        UIntVecDtype tmp_rl_entries(num_rl_entries);
#endif
        if (transform_ID_ == TransformID::ID_1) {
            for (auto idx_i = 0u; idx_i < num_rl_entries; idx_i++) {
#if defined(GENIE_CONTACT_BACKEND_EIGEN)
                tmp_rl_entries(idx_i) = reader.Read<uint8_t>();
#else
                tmp_rl_entries[idx_i] = reader.Read<uint8_t>();
#endif
            }
        } else if (transform_ID_ == TransformID::ID_2) {
            for (auto idx_i = 0u; idx_i < num_rl_entries; idx_i++) {
#if defined(GENIE_CONTACT_BACKEND_EIGEN)
                tmp_rl_entries(idx_i) = reader.Read<uint16_t>();
#else
                tmp_rl_entries[idx_i] = reader.Read<uint16_t>();
#endif
            }
        } else if (transform_ID_ == TransformID::ID_3) {
            for (auto idx_i = 0u; idx_i < num_rl_entries; idx_i++) {
#if defined(GENIE_CONTACT_BACKEND_EIGEN)
                tmp_rl_entries(idx_i) = reader.Read<uint32_t>();
#else
                tmp_rl_entries[idx_i] = reader.Read<uint32_t>();
#endif
            }
        } else {
            UTILS_DIE("This should never be reached!");
        }

        SetRlEntries(transform_ID_, first_val_, tmp_rl_entries);
    }

    reader.FlushHeldBits();
}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixMaskPayload::SubcontactMatrixMaskPayload(
    BinVecDtype&& _mask_array
)
    : transform_ID_(TransformID::ID_0),
      mask_array_(),
      first_val_(),
      rl_entries_() {
  SetMaskArray(_mask_array);
}

// ---------------------------------------------------------------------------------------------------------------------

SubcontactMatrixMaskPayload::SubcontactMatrixMaskPayload(
    TransformID _transform_ID,
    bool _first_val,
    const std::vector<uint32_t>& _rl_entries
)
    : transform_ID_(_transform_ID), mask_array_(), first_val_(_first_val) {
    UTILS_DIE_IF(_transform_ID == TransformID::ID_0, "Invalid transform_ID_!");
    rl_entries_ = _rl_entries;
}

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixMaskPayload::operator==(const SubcontactMatrixMaskPayload& other) {
    return transform_ID_ == other.transform_ID_ &&
         mask_array_ == other.mask_array_ && first_val_ == other.first_val_ &&
         rl_entries_ == other.rl_entries_;
}

// ---------------------------------------------------------------------------------------------------------------------

TransformID SubcontactMatrixMaskPayload::GetTransformID() const { return transform_ID_; }

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixMaskPayload::AnyMaskArray() const { return mask_array_.has_value(); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<bool>& SubcontactMatrixMaskPayload::GetMaskArray() const {
    UTILS_DIE_IF(
        !AnyMaskArray(),
        "mask_array_ is not set!");
    return *mask_array_;
}

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixMaskPayload::GetFirstVal() const { return first_val_; }

// ---------------------------------------------------------------------------------------------------------------------

bool SubcontactMatrixMaskPayload::AnyRlEntries() const { return rl_entries_.has_value(); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint32_t>& SubcontactMatrixMaskPayload::GetRlEntries() const {
    UTILS_DIE_IF(
        !AnyRlEntries(),
        "mask_array_ is not set!");
    return *rl_entries_;
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixMaskPayload::SetTransformId(TransformID id) {
  transform_ID_ = id; }

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixMaskPayload::SetMaskArray(
    const std::optional<BinVecDtype>& opt_array
) {
  transform_ID_ = TransformID::ID_0;
    if (rl_entries_.has_value()) {
      rl_entries_ = {};
    }

    if (opt_array.has_value()) {
#if defined(GENIE_CONTACT_BACKEND_XTENSOR)
        UTILS_DIE_IF(opt_array->shape(0) == 0, "Invalid opt_array size!");

        auto& array = opt_array.value();
        auto std_array = std::vector<bool>(array.begin(), array.end());
        first_val_ = std_array[0];
        mask_array_ = std::move(std_array);
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
        UTILS_DIE_IF(opt_array->size() == 0, "Invalid opt_array size!");
        auto& array = opt_array.value();
        std::vector<bool> std_array(array.size());
        for (int idx_i = 0; idx_i < array.size(); ++idx_i) std_array[idx_i] = array(idx_i);
        first_val_ = std_array[0];
        mask_array_ = std::move(std_array);
#else
        UTILS_DIE_IF(opt_array->size() == 0, "Invalid opt_array size!");
        first_val_ = (*opt_array)[0];
        mask_array_ = opt_array;
#endif
    } else {
      first_val_ = false;
        mask_array_ = {};
    }
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void SubcontactMatrixMaskPayload::SetFirstVal(bool val) {
    UTILS_DIE_IF(transform_ID_ == TransformID::ID_0,
        "first_val_ can only be set manually for transform_ID_ 1-3!");

    first_val_ = val;
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixMaskPayload::SetRlEntries(
    TransformID _transform_ID,
    bool _first_val,
    const std::optional<std::vector<uint32_t>>& _rl_entries
) {
    UTILS_DIE_IF(_transform_ID == TransformID::ID_0, "transform_ID_ 0 is not allowed here!");
    transform_ID_ = _transform_ID;

    if (mask_array_.has_value()) {
      mask_array_ = {};
    }

    if (_rl_entries.has_value()) {
        first_val_ = _first_val;
        rl_entries_ = _rl_entries;
    } else {
      first_val_ = false;
      rl_entries_ = {};
    }
}

void SubcontactMatrixMaskPayload::SetRlEntries(
    TransformID _transform_ID,
    bool _first_val,
    const UIntVecDtype& _rl_entries
) {
#if defined(GENIE_CONTACT_BACKEND_EIGEN)
    std::vector<uint32_t> std_rl_entries(_rl_entries.size());
    for (int i = 0; i < _rl_entries.size(); ++i) std_rl_entries[i] = _rl_entries(i);
    SetRlEntries(_transform_ID, _first_val, std::optional<std::vector<uint32_t>>(std::move(std_rl_entries)));
#elif defined(GENIE_CONTACT_BACKEND_XTENSOR)
    std::vector<uint32_t> std_rl_entries(_rl_entries.begin(), _rl_entries.end());
    SetRlEntries(_transform_ID, _first_val, std::optional<std::vector<uint32_t>>(std::move(std_rl_entries)));
#else
    // For STD backend, UIntVecDtype IS std::vector<uint32_t>.
    // We must call the version that takes std::optional<std::vector<uint32_t>>.
    SetRlEntries(_transform_ID, _first_val, std::optional<std::vector<uint32_t>>(_rl_entries));
#endif
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SubcontactMatrixMaskPayload::GetSize() const {
    size_t size_bits = TRANSFORM_ID_BLEN;  // transform_ID_
    if (transform_ID_ == TransformID::ID_0) {
        UTILS_DIE_IF(!mask_array_.has_value(), "mask_array_ is missing?");
        size_bits += mask_array_.value().size();
    } else {
        UTILS_DIE_IF(!rl_entries_.has_value(), "rl_entries_ is missing?");
        size_bits += FIRST_VAL_BLEN;
        size_bits += NUM_RL_ENTRIES_BLEN;
        size_bits += rl_entries_.value().size() * (4u << static_cast<uint8_t>(transform_ID_));
    }

    return static_cast<size_t>(ceil(static_cast<double>(size_bits) / 8));
}

// ---------------------------------------------------------------------------------------------------------------------

void SubcontactMatrixMaskPayload::Write(util::BitWriter &writer) const {
    UTILS_DIE_IF(
        !mask_array_.has_value() && !rl_entries_.has_value(),
        "Both mask_array_ and rl_entries_ are empty!");

    // Write everything on-memory before dumping it to the writer
    auto payload = std::stringstream();
    auto onmem_writer = genie::util::BitWriter(payload);

    onmem_writer.WriteBits(static_cast<uint64_t>(transform_ID_), TRANSFORM_ID_BLEN);

    if (transform_ID_ == TransformID::ID_0) {
        UTILS_DIE_IF(!mask_array_.has_value(), "mask_array_ is missing?");
        auto num_bin_entries = mask_array_->size();

        for (auto idx_i = 0u; idx_i < num_bin_entries; idx_i++) {
            auto _mask_array = mask_array_.value();
            uint64_t val = mask_array_->at(idx_i);
            onmem_writer.WriteBits(val, 1);
        }
    } else {
        UTILS_DIE_IF(!rl_entries_.has_value(), "rl_entries_ is missing?");
        onmem_writer.WriteBits(first_val_, FIRST_VAL_BLEN);
        auto num_rl_entries = rl_entries_->size();
        onmem_writer.WriteBits(num_rl_entries, NUM_RL_ENTRIES_BLEN);
        for (auto idx_i = 0u; idx_i < num_rl_entries; idx_i++) {
            auto nbits = static_cast<uint8_t>(4u << static_cast<uint8_t>(transform_ID_));
            auto val = static_cast<uint64_t>(rl_entries_->at(idx_i));
            onmem_writer.WriteBits(val, nbits);
        }
    }
    // Align to byte
    onmem_writer.FlushBits();

    auto payload_str = payload.str();
    for (auto& v : payload_str) {
        writer.WriteAlignedInt(v);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::contact

// ---------------------------------------------------------------------------------------------------------------------
