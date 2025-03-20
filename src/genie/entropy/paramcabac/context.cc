/**
 * Copyright 2018-2024 The Genie Authors.
 * @file context.cc
 * @brief Implementation of CABAC context handling for Genie.
 *
 * Manages adaptive mode, context initialization values, and serialization,
 * with support for JSON conversion and shared sub-symbol context flags.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/paramcabac/context.h"

#include <vector>

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

// -----------------------------------------------------------------------------

Context::Context() : Context(true, 8, 8, false) {}

// -----------------------------------------------------------------------------

Context::Context(const bool adaptive_mode_flag,
                 const uint8_t output_symbol_size,
                 const uint8_t coding_subsym_size, bool share_subsym_ctx_flag)
    : adaptive_mode_flag_(adaptive_mode_flag),
      num_contexts_(0),
      context_initialization_value_(0) {
  if (coding_subsym_size < output_symbol_size) {
    share_subsym_ctx_flag_ = share_subsym_ctx_flag;
  }
}

// -----------------------------------------------------------------------------

Context::Context(const uint8_t output_symbol_size,
                 const uint8_t coding_subsym_size, util::BitReader& reader) {
  adaptive_mode_flag_ = reader.Read<bool>(1);
  num_contexts_ = reader.Read<uint16_t>();
  for (size_t i = 0; i < num_contexts_; ++i) {
    context_initialization_value_.emplace_back(reader.Read<uint8_t>(7));
  }
  if (coding_subsym_size < output_symbol_size) {
    share_subsym_ctx_flag_ = reader.Read<bool>(1);
  }
}

// -----------------------------------------------------------------------------

void Context::AddContextInitializationValue(
    const uint8_t context_initialization_value) {
  ++num_contexts_;
  this->context_initialization_value_.push_back(context_initialization_value);
}

// -----------------------------------------------------------------------------

void Context::write(util::BitWriter& writer) const {
  writer.WriteBits(adaptive_mode_flag_, 1);
  writer.WriteBits(num_contexts_, 16);
  for (auto& i : context_initialization_value_) {
    writer.WriteBits(i, 7);
  }
  if (share_subsym_ctx_flag_) {
    writer.WriteBits(*share_subsym_ctx_flag_, 1);
  }
}

// -----------------------------------------------------------------------------

bool Context::GetAdaptiveModeFlag() const { return adaptive_mode_flag_; }

// -----------------------------------------------------------------------------

uint16_t Context::GetNumContexts() const { return num_contexts_; }

// -----------------------------------------------------------------------------

bool Context::GetShareSubsymCtxFlag() const {
  if (share_subsym_ctx_flag_) return *share_subsym_ctx_flag_;
  return false;
  // default.
}

// -----------------------------------------------------------------------------

const std::vector<uint8_t>& Context::GetContextInitializationValue() const {
  return context_initialization_value_;
}

// -----------------------------------------------------------------------------

bool Context::operator==(const Context& ctx) const {
  return adaptive_mode_flag_ == ctx.adaptive_mode_flag_ &&
         num_contexts_ == ctx.num_contexts_ &&
         context_initialization_value_ == ctx.context_initialization_value_ &&
         share_subsym_ctx_flag_ == ctx.share_subsym_ctx_flag_;
}

// -----------------------------------------------------------------------------

Context::Context(nlohmann::json j) : num_contexts_(0) {
  adaptive_mode_flag_ = static_cast<bool>(j["adaptive_mode_flag"]);
  if (j.contains("context_initialization_value")) {
    for (const auto& i : j["context_initialization_value"]) {
      context_initialization_value_.emplace_back(i);
    }
  }
  if (j.contains("share_subsym_ctx_flag")) {
    share_subsym_ctx_flag_ = j["share_subsym_ctx_flag"];
  }
}

// -----------------------------------------------------------------------------

nlohmann::json Context::ToJson() const {
  nlohmann::json ret;
  ret["adaptive_mode_flag"] = adaptive_mode_flag_;
  if (!context_initialization_value_.empty()) {
    ret["context_initialization_value"] = context_initialization_value_;
  }
  if (share_subsym_ctx_flag_ != std::nullopt) {
    ret["share_subsym_ctx_flag"] = *share_subsym_ctx_flag_;
  }
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------