/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_SIGNATURE_CFG_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_SIGNATURE_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/format/mpegg_p1/dataset/dataset_header.h>
#include <memory>
#include <string>
#include <vector>
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class SignatureCfg {
 private:
    std::vector<uint8_t> U_signature_length;    //!< @brief
    std::vector<uint64_t> U_cluster_signature;  //!< @brief

    uint8_t U_signature_size;  //!< @brief

 public:
    /**
     * @brief
     * @param reader
     * @param dhd
     */
    explicit SignatureCfg(util::BitReader& reader, DatasetHeader& dhd);

    /**
     * @brief
     */
    virtual ~SignatureCfg() = default;

    /**
     * @brief
     * @return
     */
    bool getUSignatureConstantLength() const;

    /**
     * @brief
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_SIGNATURE_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
