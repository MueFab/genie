/**
 * Copyright 2018-2024 The Genie Authors.
 * @file drain.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the Drain interface for processing data chunks.
 *
 * This file contains the declaration of the Drain class template, which serves
 * as an interface for data processing modules. It defines the basic functions
 * required for consuming, skipping, and flushing data chunks of a specified
 * type. Additionally, it provides the `Section` structure to describe segments
 * of data to be processed.
 *
 * @details The Drain interface is designed for modules that need to handle data
 * in a flexible manner, allowing for multithreaded processing, chunk-based data
 * flow, and the ability to skip or Flush data when necessary. It is a key
 * component in the data processing pipeline and ensures proper handling of
 * genomic records or other structured data.
 */

#ifndef SRC_GENIE_UTIL_DRAIN_H_
#define SRC_GENIE_UTIL_DRAIN_H_

// -----------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Represents a section of a genomic record to be processed.
 *
 * The Section structure defines a segment of data within a genomic record,
 * specified by a starting position (`start`) and a length (`length`). The
 * `strongSkip` flag determines whether this section should be skipped in cases
 * where the pipeline branches and later merges, ensuring that certain sections
 * are preserved or discarded as needed.
 */
struct Section {
  size_t start;      //!< @brief ID of the first record in the section.
  size_t length;     //!< @brief Number of records in this section.
  bool strong_skip;  //!< @brief Indicates if this section should be strongly
                     //!< skipped in branching pipelines.
};

/**
 * @brief Interface for data processing modules that handle data of a specific
 * type.
 *
 * The Drain template class is an abstract interface that signals the ability of
 * a module to process data of the specified type `TYPE`. It provides methods
 * for accepting data chunks, signaling the end of data, and handling skipped
 * data segments. This interface ensures that any derived classes implement the
 * required data flow operations.
 *
 * @tparam Type The type of input data that this Drain instance accepts and
 * processes.
 */
template <typename Type>
class Drain {
 public:
  /**
   * @brief Accepts a chunk of data for processing.
   *
   * This function is called to pass a data chunk to the module for
   * processing. The `flowIn` function should handle the data based on the
   * current block identifier, which is useful for managing multithreaded data
   * flows.
   *
   * @param t The input data chunk of type `TYPE`.
   * @param id The identifier for the current block, represented as a
   * `Section`.
   */
  virtual void FlowIn(Type&& t, const Section& id) = 0;

  /**
   * @brief Signals that no more data will be provided.
   *
   * This function is called when the upstream module signals that no further
   * data chunks will be sent via `flowIn`. Implementations should handle the
   * end of the data stream and perform any necessary cleanup.
   *
   * @param pos Reference to a position counter that tracks the processing
   * position.
   */
  virtual void FlushIn(uint64_t& pos) = 0;

  /**
   * @brief Virtual destructor for safe polymorphic use.
   *
   * This ensures that derived classes are properly destructed when handled
   * through a pointer to the Drain interface.
   */
  virtual ~Drain() = default;

  /**
   * @brief Skips a chunk of data without processing it.
   *
   * This function is called to indicate that a specific section of the data
   * will not be delivered using `flowIn`. Implementations should handle
   * skipped data, ensuring that the processing state is updated accordingly.
   *
   * @param id The position of the skipped data block, represented as a
   * `Section`.
   */
  virtual void SkipIn(const Section& id) = 0;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_DRAIN_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
