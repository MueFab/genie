
#include "label.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Label::Label()
    : label_ID() {}

// ---------------------------------------------------------------------------------------------------------------------

Label::Label(std::string& _label_ID)
    : label_ID(std::move(_label_ID)){}

// ---------------------------------------------------------------------------------------------------------------------

void Label::addDatasetInfo(DatasetInfo&& _ds_info) {dataset_infos.push_back(std::move(_ds_info));}

// ---------------------------------------------------------------------------------------------------------------------

void Label::addDatasetInfos(std::vector<DatasetInfo>& _ds_infos) {
    std::move(_ds_infos.begin(), _ds_infos.end(), std::back_inserter(dataset_infos));
}

// ---------------------------------------------------------------------------------------------------------------------

void Label::setDatasetInfos(std::vector<DatasetInfo>&& _ds_infos) {dataset_infos = _ds_infos;}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Label::getLabelID() const { return label_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Label::getNumDatasets() const { return (uint16_t) dataset_infos.size(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Label::getLength() const {

    // key c(4), Length u(64)
    uint64_t bitlen = (4 * sizeof(char) + 8) * 8;   //gen_info

    // label_ID st(v)
    bitlen += (label_ID.size() + 1) * 8;

    // num_datasets u(16)
    bitlen += 16;

    // for num_datasets
    for (auto& ds_info: dataset_infos){
        bitlen += ds_info.getBitLength();
    }

    // aligned to byte
    bitlen += bitlen % 8;
    // Convert to byte
    bitlen /= 8;

    return bitlen;
}

// ---------------------------------------------------------------------------------------------------------------------

void Label::write(util::BitWriter& bit_writer) const {
    // KLV (Key Length Value) format

    // Key of KVL format
    bit_writer.write("lbll");

    // Length of KVL format
    bit_writer.write(getLength(), 64);

    // label_ID st(v)
    bit_writer.write(label_ID);

    // num_datasets u(16)
    bit_writer.write(getNumDatasets(), 16);

    // for num_datasets
    for (auto& dataset_info: dataset_infos){
        dataset_info.writeToFile(bit_writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
