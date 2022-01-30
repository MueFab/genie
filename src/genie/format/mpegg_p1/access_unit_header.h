/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_HEADER_H_

#include <memory>
#include <string>
#include <vector>
#include "dataset_header.h"
#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/core/record/class-type.h"
#include "genie/format/mgb/access_unit.h"
#include "genie/format/mgb/au_type_cfg.h"
#include "genie/format/mgb/mm_cfg.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
#include "signature_cfg.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class AccessUnitHeader : public GenInfo {
 private:
    genie::format::mgb::AUHeader header;
    bool mit_flag;

 public:

    const genie::format::mgb::AUHeader& getHeader() const {
        return header;
    }

    AccessUnitHeader () : AccessUnitHeader(genie::format::mgb::AUHeader(), false) {

    }

    explicit AccessUnitHeader(util::BitReader& reader, const std::map<size_t, core::parameter::EncodingSet> &parameterSets, bool mit) : mit_flag(mit){
        reader.readBypassBE<uint64_t>();
        header = genie::format::mgb::AUHeader(reader, parameterSets, !mit_flag);

    }

    explicit AccessUnitHeader(genie::format::mgb::AUHeader _header, bool _mit_flag)
        : header(std::move(_header)), mit_flag(_mit_flag) {}

    void write(genie::util::BitWriter& bitWriter) const override {
        GenInfo::write(bitWriter);
        header.write(bitWriter, !mit_flag);
    }

    uint64_t getSize() const override {
        std::stringstream stream;
        genie::util::BitWriter writer(&stream);
        write(writer);
        return stream.str().length();
    }

    const std::string& getKey() const override {
        static const std::string key = "auhd";
        return key;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_ACCESS_UNIT_ACCESS_UNIT_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
