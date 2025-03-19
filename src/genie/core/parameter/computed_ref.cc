/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/parameter/computed_ref.h"

#include <utility>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter {

// -----------------------------------------------------------------------------

bool ComputedRef::operator==(const ComputedRef& cr) const {
  return cr_alg_id_ == cr.cr_alg_id_ && extension_ == cr.extension_;
}

// -----------------------------------------------------------------------------

ComputedRef::ComputedRef(const Algorithm cr_alg_id) : cr_alg_id_(cr_alg_id) {
  if (cr_alg_id_ == Algorithm::kPushIn ||
      cr_alg_id_ == Algorithm::kLocalAssembly) {
    extension_ = ComputedRefExtended(0, 0);
  }
}

// -----------------------------------------------------------------------------

ComputedRef::ComputedRef(util::BitReader& reader) {
  cr_alg_id_ = static_cast<Algorithm>(reader.Read<uint8_t>());
  if (cr_alg_id_ == Algorithm::kPushIn ||
      cr_alg_id_ == Algorithm::kLocalAssembly) {
    const auto pad = reader.Read<uint8_t>();
    const auto buffer = reader.Read<uint32_t>(24);
    extension_ = ComputedRefExtended(pad, buffer);
  }
}

// -----------------------------------------------------------------------------

void ComputedRef::SetExtension(ComputedRefExtended&& computed_reference) {
  UTILS_DIE_IF(!extension_,
               "Invalid computed reference mode in computed reference "
               "parameter extension");
  extension_ = computed_reference;
}

// -----------------------------------------------------------------------------

const ComputedRefExtended& ComputedRef::GetExtension() const {
  return *extension_;
}

// -----------------------------------------------------------------------------

ComputedRef::Algorithm ComputedRef::GetAlgorithm() const { return cr_alg_id_; }

// -----------------------------------------------------------------------------

void ComputedRef::Write(util::BitWriter& writer) const {
  writer.WriteBits(static_cast<uint8_t>(cr_alg_id_), 8);
  if (extension_) {
    extension_->Write(writer);
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------