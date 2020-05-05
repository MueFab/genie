/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "context.h"
#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

Context::Context() : Context(true, 8, 8, false) {}

// ---------------------------------------------------------------------------------------------------------------------

Context::Context(bool _adaptive_mode_flag,
                 uint8_t _output_symbol_size,
                 uint8_t _coding_subsym_size,
                 bool _share_subsym_ctx_flag)
    : adaptive_mode_flag(_adaptive_mode_flag),
      num_contexts(0),
      context_initialization_value(0),
      share_subsym_ctx_flag() {
    if (_coding_subsym_size < _output_symbol_size) {
        share_subsym_ctx_flag = _share_subsym_ctx_flag;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Context::Context(uint8_t output_symbol_size,
                 uint8_t coding_subsym_size,
                 util::BitReader& reader) {
    adaptive_mode_flag = reader.read<bool>(1);
    num_contexts = reader.read<uint16_t>();
    for (size_t i = 0; i < num_contexts; ++i) {
        context_initialization_value.emplace_back(reader.read<uint8_t>(7));
    }
    if (coding_subsym_size < output_symbol_size) {
        share_subsym_ctx_flag = reader.read<bool>(1);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Context::addContextInitializationValue(uint8_t _context_initialization_value) {
    ++num_contexts;
    this->context_initialization_value.push_back(_context_initialization_value);
}

// ---------------------------------------------------------------------------------------------------------------------

void Context::write(util::BitWriter& writer) const {
    writer.write(adaptive_mode_flag, 1);
    writer.write(num_contexts, 16);
    for (auto& i : context_initialization_value) {
        writer.write(i, 7);
    }
    if (share_subsym_ctx_flag) {
        writer.write(*share_subsym_ctx_flag, 1);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool Context::getAdaptiveModeFlag() const { return adaptive_mode_flag; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Context::getNumContexts() const { return num_contexts; }

// ---------------------------------------------------------------------------------------------------------------------

bool Context::getShareSubsymCtxFlag() const {
    if (share_subsym_ctx_flag)
        return *share_subsym_ctx_flag;
    else
        return false; // default.
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint8_t>& Context::getContextInitializationValue() const { return context_initialization_value; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
