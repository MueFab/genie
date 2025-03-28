/**
 * Copyright 2018-2024 The Genie Authors.
 * @file source.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the Source class template for handling data output in a
 * pipeline.
 *
 * This file contains the declaration of the `Source` class template, which
 * serves as an interface for components that generate and propagate data chunks
 * of a specified type. It provides methods for setting a downstream `Drain` to
 * receive the data and propagating data or control signals to this downstream
 * component.
 *
 * @details The `Source` class template is designed for use in a data processing
 * pipeline, where it acts as a producer of data chunks that can be consumed by
 * a `Drain`. Implementing classes can use the provided `flowOut()`,
 * `flushOut()`, and `skipOut()` methods to send data and signals to the
 * connected `Drain`.
 */

#ifndef SRC_GENIE_UTIL_SOURCE_H_
#define SRC_GENIE_UTIL_SOURCE_H_

// -----------------------------------------------------------------------------

#include "genie/util/drain.h"

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Interface for components that produce and output data chunks of a
 * specified type.
 *
 * The `Source` class template provides a standard interface for components that
 * act as data sources within a processing pipeline. It allows for setting a
 * downstream `Drain` to which data can be propagated using the `flowOut()`,
 * `flushOut()`, and `skipOut()` methods. Implementing this interface signals
 * that the class can generate and output data of type `TYPE`.
 *
 * @tparam Type The type of the data chunks produced by this source.
 */
template <typename Type>
class Source {
 protected:
  /// Pointer to the downstream `Drain` where output data will be sent.
  Drain<Type>* drain_{nullptr};

  /**
   * @brief Propagates a data chunk to the connected drain.
   *
   * This function sends the given data chunk (`t`) to the connected `Drain`
   * using the specified section identifier (`id`). It is used by the
   * implementing classes to forward data to the downstream component.
   *
   * @param t The data chunk to be propagated.
   * @param id The section identifier for tracking the data flow (used for
   * multithreading).
   */
  void FlowOut(Type&& t, const Section& id);

  /**
   * @brief Propagates an end-of-data signal to the connected drain.
   *
   * This function sends an end-of-data signal to the connected `Drain`,
   * indicating that no more data chunks will be produced. It updates the
   * provided position counter (`pos`) to reflect the final position in the
   * data stream.
   *
   * @param pos Reference to the position counter to be updated.
   */
  void FlushOut(uint64_t& pos);

  /**
   * @brief Propagates a skip signal to the connected drain.
   *
   * This function sends a skip signal to the connected `Drain`, indicating
   * that a specific section of data should be skipped. It uses the given
   * section identifier (`id`) to track the skipped section.
   *
   * @param id The section identifier for the data to be skipped.
   */
  void SkipOut(const Section& id);

 public:
  /**
   * @brief Sets the drain for propagating data output.
   *
   * This function configures the downstream component (`Drain`) that will
   * receive the output data generated by this `Source`. It allows for dynamic
   * reconfiguration of the data flow within a pipeline.
   *
   * @param d Pointer to the `Drain` to be set as the downstream data
   * consumer.
   */
  virtual void SetDrain(Drain<Type>* d);

  /**
   * @brief Virtual destructor for safe polymorphic use.
   *
   * Ensures that derived classes are properly destructed when handled through
   * a pointer to the `Source` interface.
   */
  virtual ~Source() = default;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#include "genie/util/source.impl.h"  // NOLINT

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_SOURCE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
