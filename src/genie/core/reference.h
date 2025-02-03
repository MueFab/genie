/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_REFERENCE_H_
#define SRC_GENIE_CORE_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

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
    const std::string& getName() const;

    /**
     * @brief
     * @return
     */
    uint64_t getStart() const;

    /**
     * @brief
     * @return
     */
    uint64_t getEnd() const;

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

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
