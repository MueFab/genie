
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

    // label_ID st(v)
    uint64_t len = (label_ID.size() + 1);

    // num_datasets u(16)
    len += 2;

    // for num_datasets
    for (auto& ds_info: dataset_infos){
        len += ds_info.getLength();
    }

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void Label::write(util::BitWriter& bit_writer) const {

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
