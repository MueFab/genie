#include "genie/util/exception.h"
#include "dataset_region.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

mpegg_p1::DatasetRegion::DatasetRegion(uint16_t _seq_ID, uint64_t _start_pos, uint64_t _end_pos)
    : seq_ID((_seq_ID)),
      start_pos(_start_pos),
      end_pos(_end_pos){}

void DatasetRegion::addClassID(uint8_t _class_ID) {
    class_IDs.push_back(_class_ID);
}

void DatasetRegion::addClassIDs(std::vector<uint8_t>& _class_IDs) {
    std::move(_class_IDs.begin(), _class_IDs.end(), std::back_inserter(class_IDs));
}

void DatasetRegion::setClassIDs(std::vector<uint8_t>&& _class_IDs) {
    class_IDs = _class_IDs;
}

uint16_t DatasetRegion::getSeqID() const { return seq_ID; }

uint8_t DatasetRegion::getNumClasses() const { return class_IDs.size(); }

const std::vector<uint8_t>& DatasetRegion::getClassIDs() const { return class_IDs; }

uint64_t DatasetRegion::getStartPos() const { return start_pos; }

uint64_t DatasetRegion::getEndPos() const { return end_pos; }

void DatasetRegion::write(util::BitWriter& bit_writer) const {

    // seq_ID u(16)
    bit_writer.write(seq_ID, 16);

    // num_classes u(4)
    bit_writer.write(getNumClasses(), 4);

    // for class_IDs[] u(4)
    for (auto& class_ID: class_IDs){
        bit_writer.write(class_ID, 4);
    }

    // start_pos u(40)
    bit_writer.write(start_pos, 40);

    // end_pos u(40)
    bit_writer.write(end_pos, 40);

    // align to byte
    bit_writer.flush();
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie