/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CODEBOOK_H
#define GENIE_CODEBOOK_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <cstdint>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace paramqv1 {

/**
 * ISO 23092-2 Section 3.3.2.2 table 9 Lines 4 to 7
 */
class Codebook {
   private:
    // uint8_t qv_num_codebook_entries : 8;  //!< Line 4
    std::vector<uint8_t> qv_recon;  //!< Lines 5 to 7

   public:
    explicit Codebook(util::BitReader &reader);
    Codebook(uint8_t v1, uint8_t v2);

    virtual ~Codebook() = default;

    void addEntry(uint8_t entry);

    const std::vector<uint8_t> &getEntries() const;

    virtual void write(util::BitWriter &writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramqv1
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CODEBOOK_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------