/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "importer.h"
#include <genie/util/watch.h>
#include "access_unit.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(std::istream& _file, core::ReferenceManager* manager, core::RefDecoder* refd,  genie::core::stats::PerfStats* _stats) : core::ReferenceSource(manager), reader(_file), stats(_stats), factory(manager, this), ref_manager(manager), decoder(refd) {}

// ---------------------------------------------------------------------------------------------------------------------

bool Importer::pump(size_t& id, std::mutex&) {
    util::Watch watch;
    boost::optional<mgb::AccessUnit> unit;
    util::Section sec{};
    {
        std::unique_lock<std::mutex> lock_guard(lock);
        unit = factory.read(reader);
        if (!unit) {
            return false;
        }
        sec.start = id;
        sec.length = unit->getReadCount();
        id += unit->getReadCount();
    }

    flowOut(convertAU(std::move(unit.get())), sec);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
