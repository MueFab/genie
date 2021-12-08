/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#ifndef SRC_GENIE_FORMAT_FASTA_REFERENCE_H_
#define SRC_GENIE_FORMAT_FASTA_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/core/reference.h"
#include "genie/format/fasta/manager.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fasta {

/**
 * @brief
 */
class Reference : public core::Reference {
 private:
    Manager* mgr;  //!< @brief

 public:
    /**
     * @brief
     * @param name
     * @param length
     * @param m
     */
    Reference(std::string name, size_t length, Manager* m);

    /**
     * @brief
     * @param _start
     * @param _end
     * @return
     */
    std::string getSequence(uint64_t _start, uint64_t _end) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fasta
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_FASTA_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
