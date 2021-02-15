/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_IMPORTER_H_
#define SRC_GENIE_FORMAT_MGB_IMPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>
#include "genie/core/format-importer-compressed.h"
#include "genie/core/format-importer.h"
#include "genie/core/ref-decoder.h"
#include "genie/core/reference-source.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/format/mgb/data-unit-factory.h"
#include "genie/util/bitreader.h"
#include "genie/util/ordered-section.h"
#include "genie/util/original-source.h"
#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 *
 */
class Importer : public core::FormatImporterCompressed, public core::ReferenceSource {
 private:
    util::BitReader reader;               //!<
    std::mutex lock;                      //!<
    mgb::DataUnitFactory factory;         //!<
    core::ReferenceManager* ref_manager;  //!<
    core::RefDecoder* decoder;            //!<

    /**
     *
     * @param au
     * @return
     */
    core::AccessUnit convertAU(mgb::AccessUnit&& au);

 public:
    /**
     *
     * @param _file
     * @param manager
     * @param refd
     * @param refOnly
     * @param _stats
     */
    explicit Importer(std::istream& _file, core::ReferenceManager* manager, core::RefDecoder* refd, bool refOnly);

    /**
     *
     * @param id
     * @param lock
     * @return
     */
    bool pump(uint64_t& id, std::mutex& lock) override;

    /**
     *
     * @param raw
     * @param f_pos
     * @param start
     * @param end
     * @return
     */
    std::string getRef(bool raw, size_t f_pos, size_t start, size_t end);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_IMPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
