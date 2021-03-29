/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_DATA_UNIT_FACTORY_H_
#define SRC_GENIE_FORMAT_MGB_DATA_UNIT_FACTORY_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include "genie/core/parameter/parameter_set.h"
#include "genie/format/mgb/access_unit.h"
#include "genie/format/mgb/raw_reference.h"

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

#endif  // SRC_GENIE_FORMAT_MGB_DATA_UNIT_FACTORY_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
