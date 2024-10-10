/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_REFERENCE_H_
#define SRC_GENIE_CORE_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class Reference {
 private:
    std::string name;  //!< @brief
    uint64_t start;    //!< @brief
    uint64_t end;      //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::string& getName() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint64_t getStart() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint64_t getEnd() const;

    /**
     * @brief
     * @param _start
     * @param _end
     * @return
     */
    virtual std::string getSequence(uint64_t _start, uint64_t _end) = 0;

    /**
     * @brief
     */
    virtual ~Reference() = default;

    /**
     * @brief
     * @param _name
     * @param _start
     * @param _end
     */
    Reference(std::string _name, uint64_t _start, uint64_t _end);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
