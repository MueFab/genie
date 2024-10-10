/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_FASTA_EXPORTER_H_
#define SRC_GENIE_FORMAT_FASTA_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/format-exporter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::fasta {

/**
 * @brief
 */
class Exporter : public core::FormatExporter {
 private:
    core::ReferenceManager* refMgr;  //!< @brief
    std::ostream* outfile;           //!< @brief
    size_t num_threads;              //!< @brief

 public:
    /**
     * @brief
     * @param _refMgr
     * @param out
     * @param _num_threads
     */
    Exporter(core::ReferenceManager* _refMgr, std::ostream* out, size_t _num_threads);

    /**
     * @brief
     * @param id
     */
    void flushIn(uint64_t& id) override;

    /**
     * @brief
     * @param c
     * @param s
     */
    void flowIn(core::record::Chunk&& c, const util::Section& s) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::fasta

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTA_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
