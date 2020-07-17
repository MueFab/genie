/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DATA_UNIT_FACTORY_H
#define GENIE_DATA_UNIT_FACTORY_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/parameter/parameter_set.h>

#include <map>

#include "access_unit.h"
#include "raw_reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

class Importer;

/**
 *
 */
class DataUnitFactory {
   private:
    std::map<size_t, core::parameter::ParameterSet> parameters;  //!<
    core::ReferenceManager* refmgr;                              //!<
    Importer* importer;                                          //!<
    bool referenceOnly;                                          //!<

   public:
    /**
     *
     * @param mgr
     * @param _importer
     * @param ref
     */
    explicit DataUnitFactory(core::ReferenceManager* mgr, mgb::Importer* _importer, bool ref);

    /**
     *
     * @param id
     * @return
     */
    const core::parameter::ParameterSet& getParams(size_t id) const;

    /**
     *
     * @return
     */
    const std::map<size_t, core::parameter::ParameterSet>& getParams() const;

    /**
     *
     * @param bitReader
     * @return
     */
    boost::optional<AccessUnit> read(util::BitReader& bitReader);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DATA_UNIT_FACTORY_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------