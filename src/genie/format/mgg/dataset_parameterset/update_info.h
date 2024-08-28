/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_PARAMETERSET_UPDATE_INFO_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_PARAMETERSET_UPDATE_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <optional>
#include "genie/core/constants.h"
#include "genie/format/mgg/dataset_parameterset/u_signature.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {
namespace dataset_parameterset {

/**
 * @brief
 */
class UpdateInfo {
 private:
    bool multiple_alignment_flag;             //!< @brief
    bool pos_40_bits_flag;                    //!< @brief
    core::AlphabetID alphabetId;              //!< @brief
    std::optional<USignature> u_signature;  //!< @brief

 public:
    /**
     * @brief
     * @param other
     * @return
     */
    bool operator==(const UpdateInfo& other) const;

    /**
     * @brief
     * @param _multiple_alignment_flag
     * @param _pos_40_bits_flag
     * @param _alphabetId
     */
    UpdateInfo(bool _multiple_alignment_flag, bool _pos_40_bits_flag, core::AlphabetID _alphabetId);

    /**
     * @brief
     * @param reader
     */
    explicit UpdateInfo(genie::util::BitReader& reader);

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const;

    /**
     * @brief
     * @param signature
     */
    void addUSignature(USignature signature);

    /**
     * @brief
     * @return
     */
    bool getMultipleAlignmentFlag() const;

    /**
     * @brief
     * @return
     */
    bool getPos40BitsFlag() const;

    /**
     * @brief
     * @return
     */
    core::AlphabetID getAlphabetID() const;

    /**
     * @brief
     * @return
     */
    bool hasUSignature() const;

    /**
     * @brief
     * @return
     */
    const USignature& getUSignature() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace dataset_parameterset
}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_PARAMETERSET_UPDATE_INFO_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
