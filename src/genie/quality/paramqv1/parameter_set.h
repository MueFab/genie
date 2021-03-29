/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_PARAMQV1_PARAMETER_SET_H_
#define SRC_GENIE_QUALITY_PARAMQV1_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <vector>
#include "genie/quality/paramqv1/codebook.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace paramqv1 {

/**
 *
 */
class ParameterSet {
 private:
    std::vector<Codebook> qv_codebooks;  //!<

 public:
    /**
     *
     * @param ps
     * @return
     */
    bool operator==(const ParameterSet &ps) const;

    /**
     *
     * @param reader
     */
    explicit ParameterSet(util::BitReader &reader);

    /**
     *
     */
    ParameterSet() = default;

    /**
     *
     * @return
     */
    const std::vector<Codebook> &getCodebooks() const;

    /**
     *
     */
    virtual ~ParameterSet() = default;

    /**
     *
     * @param book
     */
    void addCodeBook(Codebook &&book);

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

#endif  // SRC_GENIE_QUALITY_PARAMQV1_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
