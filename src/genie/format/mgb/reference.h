/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_REFERENCE_H_
#define SRC_GENIE_FORMAT_MGB_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

class Importer;

/**
 *
 */
class Reference : public core::Reference {
 private:
    Importer* mgr;  //!<
    size_t f_pos;   //!<
    bool raw;       //!<

 public:
    /**
     *
     * @param _name
     * @param _start
     * @param _end
     * @param _mgr
     * @param filepos
     * @param _raw
     */
    Reference(const std::string& _name, size_t _start, size_t _end, Importer* _mgr, size_t filepos, bool _raw)
        : core::Reference(_name, _start, _end), mgr(_mgr), f_pos(filepos), raw(_raw) {}

    /**
     *
     * @param _start
     * @param _end
     * @return
     */
    std::string getSequence(uint64_t _start, uint64_t _end) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
