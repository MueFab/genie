/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines the `Manager` class for handling FASTA reference data.
 *
 * The `Manager` class provides an interface for accessing and managing genomic
 * reference sequences stored in FASTA format. It extends the `ReferenceSource`
 * interface, allowing it to serve as a source of reference sequences for use in
 * applications such as alignment and variant calling.
 *
 * This file is part of the Genie project, which is a software suite for
 * processing genomic data according to the MPEG-G standard. For more details,
 * refer to the LICENSE file or visit the project's repository at:
 * https://github.com/MueFab/genie.
 */

#ifndef SRC_GENIE_FORMAT_FASTA_MANAGER_H_
#define SRC_GENIE_FORMAT_FASTA_MANAGER_H_

// -----------------------------------------------------------------------------

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "genie/core/reference_source.h"
#include "genie/format/fasta/reader.h"

// -----------------------------------------------------------------------------

namespace genie::format::fasta {

/**
 * @brief Manages FASTA reference sequences for genomic analysis.
 *
 * The `Manager` class provides functionality to read and manage genomic
 * reference sequences stored in FASTA format. It utilizes the `FastaReader`
 * class to parse the FASTA and its accompanying index and checksum files,
 * allowing for efficient retrieval of subsequences. The class is thread-safe
 * and designed to be used in multithreaded environments, ensuring synchronized
 * access to shared resources.
 */
class Manager final : public core::ReferenceSource {
  FastaReader
      reader_;  //!< @brief Instance of `FastaReader` for parsing FASTA files.
  std::mutex reader_mutex_;  //!< @brief Mutex for synchronizing access to the
                             //!< reader in multi-threaded scenarios.

  /**
   * @brief Generates a vector of unique pointers to reference handles.
   *
   * This method constructs and returns reference handles for each of the
   * sequences stored in the FASTA file. These handles are used to access
   * metadata and sequence information for each individual reference.
   *
   * @return A vector of unique pointers to `core::Reference` objects
   * representing the available sequences.
   */
  std::vector<std::unique_ptr<core::Reference>> GenerateRefHandles();

 public:
  /**
   * @brief Constructs a `Manager` instance.
   *
   * Initializes the manager with the given input streams corresponding to the
   * FASTA file, its index file (.fai), and the checksum file (e.g., SHA256).
   * The manager also takes a pointer to a reference manager for managing the
   * reference data and a path for resolving relative URIs in the FASTA index.
   *
   * @param fasta Input stream for the main FASTA file containing sequence data.
   * @param fai Input stream for the .fai index file that provides fast lookups
   * for sequence locations.
   * @param sha Input stream for the checksum file containing hash values for
   * each sequence.
   * @param mgr Pointer to the `ReferenceManager` that manages reference
   * metadata and sequences.
   * @param path The base path for resolving URIs or relative paths in the FASTA
   * index.
   */
  Manager(std::istream& fasta, std::istream& fai, std::istream& sha,
          core::ReferenceManager* mgr, std::string path);

  /**
   * @brief Retrieves a map of sequence indices and their corresponding names.
   *
   * This method returns a map where each entry represents a sequence, with its
   * index in the FASTA file as the key and the sequence name as the value. This
   * allows for quick lookups of sequence names based on their position in the
   * index.
   *
   * @return A map where the key is the sequence index and the value is the
   * sequence name.
   */
  [[nodiscard]] std::map<size_t, std::string> GetSequences() const;

  /**
   * @brief Gets the length of a specified sequence.
   *
   * Given the name of a sequence, this method returns its total length. It
   * utilizes the FASTA index file to quickly retrieve this information without
   * needing to traverse the entire FASTA file.
   *
   * @param seq The name of the sequence for which to retrieve the length.
   * @return The total length of the sequence in bases.
   */
  [[nodiscard]] uint64_t GetLength(const std::string& seq) const;

  /**
   * @brief Provides metadata for the managed reference sequences.
   *
   * This method constructs and returns a `Reference` object that encapsulates
   * metadata for all the sequences managed by this `Manager`. The metadata
   * includes details such as sequence names, lengths, and their respective URIs
   * in the FASTA file.
   *
   * @return A `Reference` object containing metadata for the managed sequences.
   */
  [[nodiscard]] core::meta::Reference GetMeta() const override;

  /**
   * @brief Retrieves a subsequence from the specified reference sequence.
   *
   * Given the name of a sequence and a range defined by start and end
   * positions, this method extracts and returns the corresponding subsequence.
   * The extraction is performed using the FASTA index for efficient lookups.
   *
   * @param sequence The name of the sequence from which to extract the
   * subsequence.
   * @param start The starting position of the subsequence (0-based).
   * @param end The ending position of the subsequence (0-based, exclusive).
   * @return A string representing the subsequence extracted from the specified
   * range.
   */
  std::string GetRef(const std::string& sequence, uint64_t start, uint64_t end);
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::fasta

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTA_MANAGER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
