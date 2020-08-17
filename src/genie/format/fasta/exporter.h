/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_EXPRTER_H
#define GENIE_EXPRTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/format-exporter.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fasta {

/**
 *
 */
class Exporter : public core::FormatExporter {
   private:
    core::ReferenceManager* refMgr;  //!<
    std::ostream* outfile;           //!<
    size_t num_threads;              //!<
   public:
    /**
     *
     * @param _refMgr
     * @param out
     * @param _num_threads
     */
    Exporter(core::ReferenceManager* _refMgr, std::ostream* out, size_t _num_threads);

    /**
     *
     * @param id
     */
    void flushIn(size_t& id) override;

    /**
     *
     * @param c
     * @param s
     */
    void flowIn(core::record::Chunk&& c, const util::Section& s) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fasta
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_EXPRTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------