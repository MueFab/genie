/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_MODULE_IMP_H_
#define SRC_GENIE_CORE_MODULE_IMP_H_

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------
template <typename Tin, typename Tout>
void Module<Tin, Tout>::SkipIn(const util::Section& id) {
  util::Source<Tout>::SkipOut(id);
}

// -----------------------------------------------------------------------------
template <typename Tin, typename Tout>
void Module<Tin, Tout>::FlushIn(uint64_t& pos) {
  util::Source<Tout>::FlushOut(pos);
}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_MODULE_IMP_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
