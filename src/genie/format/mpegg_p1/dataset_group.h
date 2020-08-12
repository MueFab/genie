/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_PART1_DATASET_GROUP_H
#define GENIE_PART1_DATASET_GROUP_H

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <vector>

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>

#include "dataset_group_header.h"
#include "reference/reference.h"
#include "dataset/dataset.h"
#include "reference_metadata/reference_metadata.h"
#include "label_list/label_list.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 *
 */
class DGMetadata {
   public:
    /**
     *
     */
    DGMetadata();

    /**
     * Get length of DG metadata in bytes
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
    std::vector<uint8_t> DG_metadata_value;  //!<

};

/**
 *
 */
class DGProtection {
   public:
    /**
     *
     */
    DGProtection();

    /**
     * Get length of DG protection in bytes
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
    std::vector<uint8_t> DG_protection_value;  //!<

};

class DatasetGroup {
   private:
    /** ------------------------------------------------------------------------------------------------------------
     * ISO 23092-1 Section 6.5.1 table 8
     * ------------------------------------------------------------------------------------------------------------- */

    // ISO 23092-1 Section 6.5.1.3
    DatasetGroupHeader dataset_group_header;

    // ISO 23092-1 Section 6.5.1.3
    // optional
    std::vector<Reference> references;

    // ISO 23092-1 Section 6.5.1.4
    std::vector<ReferenceMetadata> reference_metadata;

    // ISO 23092-1 Section 6.5.1.5
    // optional
    std::unique_ptr<LabelList> label_list;

    // ISO 23092-1 Section 6.5.1.6 specification 23092-3
    // optional
    std::unique_ptr<DGMetadata> DG_metadata;

    // ISO 23092-1 Section 6.5.1.7 specification 23092-3
    // optional
    std::unique_ptr<DGProtection> DG_protection;

    // ISO 23092-1 Section 6.5.2.1
    std::vector<Dataset> datasets;

   public:

    /**
     *
     * @param _datasets
     */
    explicit DatasetGroup(std::vector<Dataset>&& _datasets);

    explicit DatasetGroup(util::BitReader& bit_reader);

    /**
     *
     * @return
     */
    const DatasetGroupHeader& getDatasetGroupHeader() const;

    /**
     *
     * @param _references
     */
    void addReferences(std::vector<Reference>&& _references);

    /**
     *
     * @return
     */
    const std::vector<Reference>& getReferences() const;

    /**
     *
     * @param _ref_metadata
     */
    void addReferenceMetadata(std::vector<ReferenceMetadata>&& _ref_metadata);

    /**
     * Get an array of reference metadata
     *
     * @return
     */
    const std::vector<ReferenceMetadata>& getReferenceMetadata() const;

    /**
     *
     * @param _labels
     */
    void addLabels(std::vector<Label>&& _labels);

    /**
     *
     * @param _label_list
     */
    void addLabelList(std::unique_ptr<LabelList> _label_list);

    /**
     *
     * @return
     */
    const LabelList& getLabelList() const;

    /**
     * Add DG_metadata
     *
     * @param _dg_metadata
     */
    void addDGMetadata(std::unique_ptr<DGMetadata> _dg_metadata);

    /**
     * Get DG_metadata
     *
     * @return
     */
    const DGMetadata& getDgMetadata() const;

    /**
     * Add DG_protection
     *
     * @param _dg_protection
     */
    void addDGProtection(std::unique_ptr<DGProtection>_dg_protection);

    /**
     * Get DG_protection
     *
     * @return
     */
    const DGProtection& getDgProtection() const;

    /**
     *
     * @return
     */
    const std::vector<Dataset>& getDatasets() const;

    /**
     *
     * @param _dataset_group_ID
     */
    void setDatasetGroupId(uint8_t _dataset_group_ID);

    /**
     * Get length of Dataset Header in bytes.
     * @return
     */
    uint64_t getLength() const;

    /**
     *
     * @param bit_writer
     */
    void write(util::BitWriter& bit_writer) const;

};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_PART1_DATASET_GROUP_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------