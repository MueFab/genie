/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CORE_REFERENCE_H
#define GENIE_CORE_REFERENCE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class Reference {
   private:
    std::string name; //!<
    uint64_t start; //!<
    uint64_t end; //!<

   public:
    /**
     *
     * @return
     */
    const std::string& getName() const;

    /**
     *
     * @return
     */
    uint64_t getStart() const;

    /**
     *
     * @return
     */
    uint64_t getEnd() const;

    /**
     *
     * @param _start
     * @param _end
     * @return
     */
    virtual std::string getSequence(uint64_t _start, uint64_t _end) = 0;

    /**
     *
     */
    virtual ~Reference() = default;

    /**
     *
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

#endif  // GENIE_REFERENCE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------