/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_FIELDS_CLASS_DESCRIPTION_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_FIELDS_CLASS_DESCRIPTION_H_

#include <string>
#include <vector>
#include <memory>
#include "genie/util/make-unique.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class ClassDescription {
 private:
    bool block_header_flag;
    uint8_t clid;
    std::vector<uint8_t> descriptor_IDs;

 public:
    explicit ClassDescription(genie::util::BitReader& reader, bool _block_header_flag);

    uint64_t getBitLength() const;

    void write(genie::util::BitWriter& writer) const;
};

}
}
}

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_FIELDS_CLASS_DESCRIPTION_H_
