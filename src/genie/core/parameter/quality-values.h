/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_QUALITY_VALUES_H_
#define SRC_GENIE_CORE_PARAMETER_QUALITY_VALUES_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

/**
 * @brief
 */
class QualityValues {
 public:
    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const = 0;

    /**
     * @brief
     */
    virtual ~QualityValues() = default;

    /**
     * @brief
     * @param _qv_coding_mode
     * @param _qv_reverse_flag
     */
    QualityValues(uint8_t _qv_coding_mode, bool _qv_reverse_flag);

    /**
     * @brief
     * @return
     */
    virtual std::unique_ptr<QualityValues> clone() const = 0;

    /**
     * @brief
     * @return
     */
    uint8_t getMode() const;

    /**
     * @brief
     * @return
     */
    virtual size_t getNumSubsequences() const = 0;

    /**
     * @brief
     * @param qv
     * @return
     */
    virtual bool equals(const QualityValues* qv) const;

 protected:
    uint8_t qv_coding_mode;  //!< @brief
    bool qv_reverse_flag;    //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_QUALITY_VALUES_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
