/**
 * Copyright 2018-2024 The Genie Authors.
 * @file reference_source.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the ReferenceSource class for managing genomic
 * references in the GENIE framework.
 *
 * This file contains the declaration of the `ReferenceSource` class, which
 * serves as a base class for managing and providing access to genomic reference
 * data. The class is designed to interact with a `ReferenceManager` and
 * provides metadata related to the reference sequences.
 *
 * @details The `ReferenceSource` class is part of the GENIE core module and
 * serves as an interface for components that handle reference data. It provides
 * a method for retrieving reference metadata and acts as a common base class
 * for various reference-handling implementations.
 */

#ifndef SRC_GENIE_CORE_REFERENCE_SOURCE_H_
#define SRC_GENIE_CORE_REFERENCE_SOURCE_H_

// -----------------------------------------------------------------------------

#include "genie/core/reference_manager.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief Base class for managing access to genomic reference data.
 *
 * The `ReferenceSource` class provides an interface for managing and retrieving
 * genomic reference sequences. It acts as a bridge between the reference
 * management system (`ReferenceManager`) and components that need to access
 * reference sequences. The class offers a method to retrieve metadata
 * associated with the references and ensures that derived classes can provide
 * additional functionalities.
 */
class ReferenceSource {
 protected:
  ReferenceManager* ref_mgr_;  //!< @brief Pointer to the `ReferenceManager`
                               //!< for managing reference sequences.

 public:
  /**
   * @brief Constructs a `ReferenceSource` object with a given reference
   * manager.
   *
   * Initializes the `ReferenceSource` with a pointer to a `ReferenceManager`,
   * which is responsible for managing the genomic references. This
   * constructor establishes the link between the reference source and the
   * manager.
   *
   * @param mgr Pointer to the `ReferenceManager` instance to be used for
   * managing references.
   */
  explicit ReferenceSource(ReferenceManager* mgr);

  /**
   * @brief Retrieves the metadata associated with the reference sequences.
   *
   * This function returns a `meta::Reference` object containing metadata
   * information about the reference sequences managed by this
   * `ReferenceSource`. The metadata includes details such as reference names,
   * lengths, and other relevant attributes.
   *
   * @return A `meta::Reference` object containing the metadata for the
   * reference sequences.
   */
  [[nodiscard]] virtual meta::Reference GetMeta() const;

  /**
   * @brief Virtual destructor for safe polymorphic use.
   *
   * Ensures that derived classes are properly destructed when handled through
   * a pointer to the `ReferenceSource` interface.
   */
  virtual ~ReferenceSource() = default;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_REFERENCE_SOURCE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
