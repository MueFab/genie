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
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::quality::paramqv1 {

/**
 * @brief
 */
class ParameterSet {
 private:
    std::vector<Codebook> qv_codebooks;  //!< @brief

 public:
    /**
     * @brief
     * @param ps
     * @return
     */
    bool operator==(const ParameterSet &ps) const;

    /**
     * @brief
     * @param reader
     */
    explicit ParameterSet(util::BitReader &reader);

    /**
     * @brief
     */
    ParameterSet() = default;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<Codebook> &getCodebooks() const;

    /**
     * @brief
     */
    virtual ~ParameterSet() = default;

    /**
     * @brief
     * @param book
     */
    void addCodeBook(Codebook &&book);

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter &writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::quality::paramqv1

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_PARAMQV1_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
