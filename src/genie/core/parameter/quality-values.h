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
 *
 */
class QualityValues {
   public:
    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter &writer) const = 0;

    /**
     *
     */
    virtual ~QualityValues() = default;

    /**
     *
     * @param _qv_coding_mode
     * @param _qv_reverse_flag
     */
    QualityValues(uint8_t _qv_coding_mode, bool _qv_reverse_flag);

    /**
     *
     * @return
     */
    virtual std::unique_ptr<QualityValues> clone() const = 0;

    /**
     *
     * @return
     */
    uint8_t getMode() const;

    /**
     *
     * @return
     */
    virtual size_t getNumSubsequences() const = 0;

   protected:
    uint8_t qv_coding_mode;  //!<
    bool qv_reverse_flag;    //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_QUALITY_VALUES_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------