/**
 * Copyright 2018-2024 The Genie Authors.
 * @file ordered_section.cc
 * @brief Implementation of the OrderedSection class for managing ordered
 * sections with locks.
 *
 * This file contains the implementation of the OrderedSection class, which
 * provides mechanisms to manage sections in a specific order using locks.
 * It ensures that sections are processed in the correct order by waiting
 * for the appropriate start position and signaling when a section is finished.
 *
 * @details The OrderedSection class constructor takes a lock and a section
 * identifier, waits for the section's start position, and signals the lock
 * when the section is finished. This helps in maintaining the order of
 * sections during processing.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#include "genie/util/ordered_section.h"

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

OrderedSection::OrderedSection(OrderedLock* lock, const Section& id)
    : lock_(lock), length_(id.length) {
  lock_->Wait(id.start);
}

// -----------------------------------------------------------------------------

OrderedSection::~OrderedSection() { lock_->Finished(length_); }

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
