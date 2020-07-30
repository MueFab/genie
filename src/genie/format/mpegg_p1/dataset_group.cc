/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "dataset_group.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

DGMetadata::DGMetadata() : DG_metadata_value() {
    DG_metadata_value = {0x37, 0xfd, 0x58, 0x7a, 0x00, 0x5a, 0x04, 0x00, 0xd6, 0xe6, 0x46,
                         0xb4, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x1c, 0x21, 0x44, 0xb6, 0x1f,
                         0x7d, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x5a, 0x59};
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DGMetadata::getLength() const {return DG_metadata_value.size();}

// ---------------------------------------------------------------------------------------------------------------------

void DGMetadata::writeToFile(util::BitWriter& bit_writer) const {
    for (auto val: DG_metadata_value){
        bit_writer.write(val, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

DGProtection::DGProtection() : DG_protection_value() {
    DG_protection_value = {0x37, 0xfd, 0x58, 0x7a, 0x00, 0x5a, 0x04, 0x00, 0xd6, 0xe6, 0x46,
                           0xb4, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x1c, 0x21, 0x44, 0xb6, 0x1f,
                           0x7d, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x5a, 0x59};
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DGProtection::getLength() const {return DG_protection_value.size();}

// ---------------------------------------------------------------------------------------------------------------------

void DGProtection::writeToFile(util::BitWriter& bit_writer) const {
    for (auto val: DG_protection_value){
        bit_writer.write(val, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

// TODO (Yeremia): Fix version number
DatasetGroup::DatasetGroup(std::vector<Dataset> &&_datasets)
    : dataset_group_header(_datasets, 0),
      datasets(std::move(_datasets)){}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::setDatasetGroupId(uint8_t _dataset_group_ID) {

    dataset_group_header.setDatasetGroupId(_dataset_group_ID);

    for (auto& ref: references){
        ref.setDatasetGroupId(_dataset_group_ID);
    }

    for (auto& ref_meta: reference_metadata){
        ref_meta.setDatasetGroupId(_dataset_group_ID);
    }

    if (label_list != nullptr){
        label_list->setDatasetGroupId(_dataset_group_ID);
    }

    // DG_metadata has no dataset_group_ID

    // DG_protection has no dataset_group_ID

    for (auto& ds: datasets){
        ds.setDatasetGroupId(_dataset_group_ID);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addReferences(std::vector<Reference>&& _references) {references = std::move(_references);}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Reference> &DatasetGroup::getReferences() const {return references;}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addReferenceMetadata(std::vector<ReferenceMetadata> &&_ref_metadata) {
    reference_metadata = std::move(_ref_metadata);
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<ReferenceMetadata> &DatasetGroup::getReferenceMetadata() const { return reference_metadata;}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addLabels(std::vector<Label> && _labels) {
    UTILS_DIE_IF(label_list != nullptr, "Label list is already added");

    label_list = util::make_unique<LabelList>(getDatasetGroupHeader().getDatasetGroupId(), std::move(_labels));
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addLabelList(std::unique_ptr<LabelList> _label_list) {
    UTILS_DIE_IF(label_list != nullptr, "Label list is already added");

    _label_list = std::move(label_list);
}

// ---------------------------------------------------------------------------------------------------------------------

const LabelList& DatasetGroup::getLabelList() const { return *label_list;}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addDGMetadata(std::unique_ptr<DGMetadata> _dg_metadata) { DG_metadata = std::move(_dg_metadata);}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addDGProtection(std::unique_ptr<DGProtection> _dg_protection) {DG_protection = std::move(_dg_protection);}


// ---------------------------------------------------------------------------------------------------------------------

const DatasetGroupHeader& DatasetGroup::getDatasetGroupHeader() const { return dataset_group_header; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Dataset>& DatasetGroup::getDatasets() const { return datasets; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetGroup::getLength() const {

    uint64_t length = dataset_group_header.getLength();

    for (auto& ref: references){
        length += ref.getLength();
    }

    for (auto& ref_meta: reference_metadata){
        length += ref_meta.getLength();
    }

    if (label_list != nullptr){
        length += label_list->getLength();
    }

    if (DG_metadata != nullptr){
        length += DG_metadata->getLength();
    }

    if (DG_metadata != nullptr){
        length += DG_metadata->getLength();
    }

    for (auto& ds: datasets){
        length += ds.getLength();
    }

    return length;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::writeToFile(genie::util::BitWriter& bit_writer) const {
    // KLV (Key Length Value) format

    bit_writer.write("dgcn");

    // key (4), Length (8)
    uint64_t length = 12;

    // TODO (Yeremia): Value?

    length += getLength();

    bit_writer.write(length, 64);

    dataset_group_header.writeToFile(bit_writer);
    for (auto const& it : datasets) {
        it.writeToFile(bit_writer);
    }

    // dataset_group_header
    dataset_group_header.writeToFile(bit_writer);

    // reference (optional)
    for (auto &reference: references){
        reference.writeToFile(bit_writer);
    }

    // reference_metadata (optional)
    for (auto &ref_metadata: reference_metadata){
        ref_metadata.writeToFile(bit_writer);
    }

    // label_list (optional)
    if (label_list != nullptr){
        label_list->writeToFile(bit_writer);
    }

    // DG_metadata (optional)
    if (DG_metadata != nullptr){
        DG_metadata->writeToFile(bit_writer);
    }

    // DG_protection (optional)
    if (DG_protection != nullptr){
        DG_protection->writeToFile(bit_writer);
    }

    // dataset[]
    for (auto& ds: datasets){
        ds.writeToFile(bit_writer);
    }

    bit_writer.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------