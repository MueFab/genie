/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#ifndef GENIE_FASTA_REFERENCE_H
#define GENIE_FASTA_REFERENCE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/reference.h>

#include "manager.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fasta {

/**
 *
 */
class Reference : public core::Reference {
   private:
    Manager* mgr;  //!<

   public:
    /**
     *
     * @param name
     * @param length
     * @param m
     */
    Reference(std::string name, size_t length, Manager* m);

    /**
     *
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

#endif  // GENIE_FASTA_REFERENCE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------