/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/stringview.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

StringView::StringView(size_t start_p, size_t stop_p) : start(start_p), stop(stop_p), memory(nullptr) {}

// ---------------------------------------------------------------------------------------------------------------------

StringView::StringView(size_t start_p, size_t stop_p, const char* mem) : start(start_p), stop(stop_p), memory(mem) {}

// ---------------------------------------------------------------------------------------------------------------------

StringView StringView::deploy(const char* mem) const { return {start, stop, mem}; }

// ---------------------------------------------------------------------------------------------------------------------

const char* StringView::begin() const { return memory + start; }

// ---------------------------------------------------------------------------------------------------------------------

const char* StringView::end() const { return memory + stop; }

// ---------------------------------------------------------------------------------------------------------------------

size_t StringView::length() const { return stop - start; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
