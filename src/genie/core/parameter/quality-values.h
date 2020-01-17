/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_QUALITY_VALUES_H
#define GENIE_QUALITY_VALUES_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/make-unique.h>
#include <cstdint>
#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

/**
 * ISO 23092-2 Section 3.3.2 table 7 lines 32 to 40
 */
class QualityValues {
   public:

    virtual void write(util::BitWriter &writer) const = 0;

    virtual ~QualityValues() = default;

    QualityValues(uint8_t _qv_coding_mode, bool _qv_reverse_flag);

    virtual std::unique_ptr<QualityValues> clone() const = 0;

   protected:
    uint8_t qv_coding_mode;       //!< : 4; Line 32
    bool qv_reverse_flag : 1;  //!< Line 40
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_QUALITY_VALUES_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------