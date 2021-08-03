/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include <memory>
#include "genie/util/make-unique.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/format/mpegg_p1/file_header.h"

#include "genie/format/mpegg_p1/dataset/dataset_header.h"
#include "genie/format/mpegg_p1/dataset/dataset_parameter_set.h"
#include "genie/format/mpegg_p1/dataset/access_unit/access_unit.h"
#include "genie/format/mpegg_p1/dataset/descriptor_stream.h"

#include "genie/core/parameter/data_unit.h"
#include "genie/format/mgb/data-unit-factory.h"
//#include "genie/format/mpegg_p1/dataset/master_index_table/master_index_table.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DTMetadata {
 public:
    /**
     * @brief
     */
    DTMetadata();

    /**
     * @brief
     * @param _DT_metadata_value
     */
    explicit DTMetadata(std::vector<uint8_t>&& _DT_metadata_value);

    /**
     * @brief
     * @param reader
     * @param length
     */
    DTMetadata(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length);

    /**
     *
     * @return
     */
    uint64_t getLength() const;

    /**
     *
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;

 private:
    std::vector<uint8_t> DT_metadata_value;  //!< @brief
};

/**
 * @brief
 */
class DTProtection {
 public:
    /**
     * @brief
     */
    DTProtection();

    /**
     * @brief
     * @param _dt_protection_value
     */
    explicit DTProtection(std::vector<uint8_t>&& _dt_protection_value);

    /**
     * @brief
     * @param reader
     * @param length
     */
    DTProtection(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length);

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;

 private:
    std::vector<uint8_t> DT_protection_value;  //!< @brief
};

/**
 * @brief
 */
class Dataset {
 private:
    /** ------------------------------------------------------------------------------------------------------------
     *  ISO 23092-1 Section 6.5.2.2 table 18
     *  ------------------------------------------------------------------------------------------------------------ */

    DatasetHeader header;  //!< @brief

    std::unique_ptr<DTMetadata> metadata;  //!< @brief ISO 23092-1 Section 6.5.2.3 - specification 23092-3

    std::unique_ptr<DTProtection> protection;  //!< @brief ISO 23092-1 Section 6.5.2.4 - specification 23092-3

    std::vector<DatasetParameterSet> parameter_sets;  //!< @brief ISO 23092-1 Section 6.5.2.5 - specification 23092-2

    std::vector<AccessUnit> access_units;  //!< @brief ISO 23092-1 Section 6.5.3

    std::vector<DescriptorStream> descriptor_streams;  //!< @brief ISO 23092-1 Section 6.5.4

 public:

    /**
     *
     */
    Dataset();

    /**
     *
     * @param reader
     * @param fhd
     * @param start_pos
     * @param length
     */
    explicit Dataset(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length);

    /**
     * @brief
     * @return
     */
    DatasetHeader& getHeader();

    /**
     * @brief
     * @return
     */
    std::vector<DatasetParameterSet>& getParameterSets();

    /**
     * @brief
     * @return
     */
    std::vector<AccessUnit>& getAccessUnits();

    /**
     * @brief
     * @return
     */
    std::vector<DescriptorStream>& getDescriptorStreams();

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param bit_writer
     */
    void write(genie::util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DATASET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
