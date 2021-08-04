/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_SIGNATURE_CFG_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_SIGNATURE_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------

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

#include <genie/format/mpegg_p1/dataset/dataset_header.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 *
 */
class SignatureCfg {
 private:

    std::vector<uint8_t> U_signature_length;  //!<
    std::vector<uint64_t> U_cluster_signature;  //!<

    uint8_t U_signature_size;  //!<

 public:
    /**
     *
     * @param _U_signature_size
     * @param multiple_signature_base
     * @param reader
     */
    explicit SignatureCfg(util::BitReader& reader, DatasetHeader& dhd);

    /**
     *
     */
    virtual ~SignatureCfg() = default;

//    /**
//     *
//     * @param _U_cluster_signature
//     */
//    void addSignature(uint64_t _U_cluster_signature);

    bool getUSignatureConstantLength() const;

    /**
     *
     * @param writer
     */
    virtual void write(util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_SIGNATURE_CFG_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
