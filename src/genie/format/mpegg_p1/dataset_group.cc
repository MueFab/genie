/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
//#include <genie/format/mpegg_p1/util.h>
#include <genie/util/runtime-exception.h>

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

DGMetadata::DGMetadata(util::BitReader& reader, size_t length) {

    std::string key = readKey(reader);
    UTILS_DIE_IF(key != "dgmd", "DGMetadata is not Found");

    size_t start_pos = reader.getPos();

    // DG_metadata_value[uint8_t]
    for (auto& val : DG_metadata_value) {
        val = reader.read<uint8_t>();
    }

    UTILS_DIE_IF(reader.getPos() - start_pos != length, "Invalid DGMetadata length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DGMetadata::getLength() const {return DG_metadata_value.size();}

// ---------------------------------------------------------------------------------------------------------------------

void DGMetadata::write(util::BitWriter& bit_writer) const {
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

DGProtection::DGProtection(util::BitReader& reader, size_t length) {

    std::string key = readKey(reader);
    UTILS_DIE_IF(key != "dgpr", "DGProtection is not Found");

    size_t start_pos = reader.getPos();

    // DG_protection_value[uint8_t]
    for (auto& val : DG_protection_value) {
        val = reader.read<uint8_t>();
    }

    UTILS_DIE_IF(reader.getPos() - start_pos != length, "Invalid DGProtection length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DGProtection::getLength() const {return DG_protection_value.size();}

// ---------------------------------------------------------------------------------------------------------------------

void DGProtection::write(util::BitWriter& bit_writer) const {
    for (auto val: DG_protection_value){
        bit_writer.write(val, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroup::DatasetGroup(std::vector<Dataset> &&_datasets)
    : dataset_group_ID(0),
      version_number(0), // FIXME: Fix version number
      datasets(std::move(_datasets)){

    std::vector<uint16_t> dataset_IDs(getDatasetIDs(true));

    auto it = std::unique( dataset_IDs.begin(), dataset_IDs.end() );

    UTILS_DIE_IF(!(it == dataset_IDs.end() ), "dataset_ID is not unique!");
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroup::DatasetGroup(util::BitReader& reader, size_t length)
    : dataset_group_ID(0),
      version_number(0) {

    size_t start_pos = reader.getPos();

    std::string key = readKey(reader);
    UTILS_DIE_IF(key != "dgcn", "DatasetGroup is not Found");

    // Class dataset_group_header
    auto header_length = reader.read<size_t>();
    readHeader(reader, header_length-12);

    do {
        key = readKey(reader);
        if (key == "rfgn"){
            // reference[]
            auto ref_length = reader.read<size_t>();
            references.emplace_back(reader, ref_length);
            key = readKey(reader);
        } else if (key == "rfmd"){
            // reference_metadata[]
//            auto rm_length = reader.read<size_t>();
//            reference_metadata.emplace_back(reader, rm_length);

            key = readKey(reader);
        } else if (key == "labl"){
            // label_list
            auto ll_length = reader.read<size_t>();
            label_list = util::make_unique<LabelList>(reader, ll_length);

            key = readKey(reader);
        } else if (key == "dgmd"){
            // DG_metadata
            auto metadata_length = reader.read<size_t>();
            DG_metadata = util::make_unique<DGMetadata>(reader, metadata_length);

            key = readKey(reader);
        } else if (key == "dgpr"){
            // DG_protection
            auto protection_length = reader.read<size_t>();
            DG_protection = util::make_unique<DGProtection>(reader, protection_length);

            key = readKey(reader);
        } else if (key == "dtcn"){
            // Dataset
            auto dataset_length = reader.read<size_t>();
            datasets.emplace_back(reader, dataset_length);
        }

    } while (reader.getPos() - start_pos < length);

    UTILS_DIE_IF(reader.getPos()-start_pos != length, "Invalid DatasetGroup length!");
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<uint16_t>&& DatasetGroup::getDatasetIDs(bool sort_ids) const {
    std::vector<uint16_t> dataset_IDs;

    for (auto &ds : datasets){
        dataset_IDs.push_back(ds.getID());
    }

    if (sort_ids){
        sort(dataset_IDs.begin(), dataset_IDs.end());
    }

    return std::move(dataset_IDs);
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

    label_list = util::make_unique<LabelList>(dataset_group_ID, std::move(_labels));
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

const DGMetadata &DatasetGroup::getDgMetadata() const {return *DG_metadata;}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addDGProtection(std::unique_ptr<DGProtection> _dg_protection) {DG_protection = std::move(_dg_protection);}

// ---------------------------------------------------------------------------------------------------------------------

const DGProtection &DatasetGroup::getDgProtection() const {return *DG_protection;}


// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Dataset>& DatasetGroup::getDatasets() const { return datasets; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::setID(uint8_t ID) {

    dataset_group_ID = ID;

    for (auto& ref: references){
        ref.setDatasetGroupId(ID);
    }

    for (auto& ref_meta: reference_metadata){
        ref_meta.setDatasetGroupId(ID);
    }

    if (label_list != nullptr){
        label_list->setDatasetGroupId(ID);
    }

    // DG_metadata has no dataset_group_ID

    // DG_protection has no dataset_group_ID

    for (auto& ds: datasets){
        ds.setGroupId(ID);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetGroup::getHeaderLength() const {

    /// Key c(4) Length u(64)
    uint64_t len = 12;

    // dataset_group_ID u(8)
    len += sizeof(uint8_t);

    // version_number u(8)
    len += sizeof(uint8_t);

    // dataset_IDs[] u(16)
    len += sizeof(uint16_t) * datasets.size();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::writeHeader(util::BitWriter& writer) const {

    /// KLV (Key Length Value) format
    // Key of KLV format
    writer.write("dghd");

    // Length of KLV format
    writer.write(getLength(), 64);

    // dataset_group_ID u(8)
    writer.write(dataset_group_ID, 8);

    // version_number u(8)
    writer.write(version_number, 8);

    // dataset_IDs[] u(16)
    for (auto &d_ID: getDatasetIDs()){
        writer.write(d_ID, 16);
    }

    writer.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::readHeader(util::BitReader& reader, size_t length) {

    size_t start_pos = reader.getPos();

    std::string key = readKey(reader);
    UTILS_DIE_IF(key != "dghd", "DatasetGroupHeader is not Found");

    dataset_group_ID = reader.read<uint8_t>();
    version_number = reader.read<uint8_t>();

    // dataset_IDs[] u(16)
    for (auto &d_ID: getDatasetIDs()){
        d_ID = reader.read<uint16_t>();
    }

    UTILS_DIE_IF(reader.getPos()-start_pos != length, "Invalid DatasetGroupHeader length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetGroup::getLength() const {

    /// Key c(4) Length u(64)
    uint64_t len = (4 * sizeof(char) + 8);   // gen_info

    len += getHeaderLength();

    for (auto& ref: references){
        len += ref.getLength();
    }

    for (auto& ref_meta: reference_metadata){
        len += ref_meta.getLength();
    }

    if (label_list != nullptr){
        len += label_list->getLength();
    }

    if (DG_metadata != nullptr){
        len += DG_metadata->getLength();
    }

    if (DG_metadata != nullptr){
        len += DG_metadata->getLength();
    }

    for (auto& ds: datasets){
        len += ds.getLength();
    }

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::write(util::BitWriter& writer) const {

    /// KLV (Key Length Value) format
    writer.write("dgcn");

    // Length of KLV format
    writer.write(getLength(), 64);

    // dataset_group_header
    writeHeader(writer);

    // reference (optional)
    for (auto &reference: references){
        reference.write(writer);
    }

    // reference_metadata (optional)
    for (auto &ref_metadata: reference_metadata){
        ref_metadata.write(writer);
    }

    // label_list (optional)
    if (label_list != nullptr){
        label_list->writeToFile(writer);
    }

    // DG_metadata (optional)
    if (DG_metadata != nullptr){
        DG_metadata->write(writer);
    }

    // DG_protection (optional)
    if (DG_protection != nullptr){
        DG_protection->write(writer);
    }

    // dataset[]
    for (auto& ds: datasets){
        ds.write(writer);
    }

    writer.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------