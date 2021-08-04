/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */


#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_HEADER_H
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_HEADER_H

#include <string>
#include <vector>
#include <memory>
#include "genie/util/make-unique.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/format/mpegg_p1/dataset/class_description.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class DatasetGroupHeader {
 private:
    /** ------------------------------------------------------------------------------------------------------------
    * ISO 23092-1 Section 6.5.1.2 table 9 - dataset_group_header
    * ------------------------------------------------------------------------------------------------------------- */
    uint8_t ID;
    uint8_t version;
    std::vector<uint16_t> dataset_IDs;

 public:

    /**
     *
     * @param reader
     * @param fhd
     * @param start_pos
     * @param length
     */
    DatasetGroupHeader();

    /**
     *
     * @param reader
     * @param fhd
     * @param start_pos
     * @param length
     */
    explicit DatasetGroupHeader(genie::util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length);

    /**
     *
     * @return
     */
    uint8_t getID() const;

    /**
     *
     * @param _ID
     */
    void setID(uint8_t _ID);

    /**
     *
     * @return
     */
    uint8_t getVersion() const;

    /**
     *
     * @return
     */
    std::vector<uint16_t>& getDatasetIDs();

    /**
     *
     * @return
     */
    uint64_t getLength() const;

    /**
     *
     * @param writer
     */
    void write(genie::util::BitWriter& writer, bool empty_length=false) const;
};

}
}
}

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_HEADER_H
