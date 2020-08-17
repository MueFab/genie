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
 *
 */
class Codebook {
   private:
    std::vector<uint8_t> qv_recon;  //!<

   public:
    /**
     *
     * @param ps
     * @return
     */
    bool operator==(const Codebook &ps) const;

    /**
     *
     * @param reader
     */
    explicit Codebook(util::BitReader &reader);

    /**
     *
     * @param v1
     * @param v2
     */
    Codebook(uint8_t v1, uint8_t v2);

    /**
     *
     */
    virtual ~Codebook() = default;

    /**
     *
     * @param entry
     */
    void addEntry(uint8_t entry);

    /**
     *
     * @return
     */
    const std::vector<uint8_t> &getEntries() const;

    /**
     *
     * @param writer
     */
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