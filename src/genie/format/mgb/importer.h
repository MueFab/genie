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
#include "genie/util/bit_reader.h"
#include "genie/util/ordered_section.h"
#include "genie/util/original_source.h"
#include "genie/util/source.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 * @brief
 */
class Importer : public core::FormatImporterCompressed, public core::ReferenceSource {
 private:
    util::BitReader reader;               //!< @brief
    std::mutex lock;                      //!< @brief
    mgb::DataUnitFactory factory;         //!< @brief
    core::ReferenceManager* ref_manager;  //!< @brief
    core::RefDecoder* decoder;            //!< @brief

    /**
     * @brief
     * @param au
     * @return
     */
    core::AccessUnit convertAU(mgb::AccessUnit&& au);

 public:
    /**
     * @brief
     * @param _file
     * @param manager
     * @param refd
     * @param refOnly
     */
    explicit Importer(std::istream& _file, core::ReferenceManager* manager, core::RefDecoder* refd, bool refOnly);

    /**
     * @brief
     * @param id
     * @param lock
     * @return
     */
    bool Pump(uint64_t& id, std::mutex& lock) override;

    /**
     * @brief
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
