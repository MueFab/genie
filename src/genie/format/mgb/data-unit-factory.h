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

namespace genie::format::mgb {

// ---------------------------------------------------------------------------------------------------------------------

class Importer;

/**
 * @brief
 */
class DataUnitFactory {
 private:
    std::map<size_t, core::parameter::EncodingSet> parameters;  //!< @brief
    core::ReferenceManager* refmgr;                             //!< @brief
    Importer* importer;                                         //!< @brief
    bool referenceOnly;                                         //!< @brief

 public:
    /**
     * @brief
     * @param mgr
     * @param _importer
     * @param ref
     */
    explicit DataUnitFactory(core::ReferenceManager* mgr, mgb::Importer* _importer, bool ref);

    /**
     * @brief
     * @param id
     * @return
     */
    [[nodiscard]] const core::parameter::EncodingSet& getParams(size_t id) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::map<size_t, core::parameter::EncodingSet>& getParams() const;

    /**
     * @brief
     * @param bitReader
     * @return
     */
    std::optional<AccessUnit> read(util::BitReader& bitReader);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgb

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_DATA_UNIT_FACTORY_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
