#include "sam_sorter.h"

#include <iostream>

namespace sam_transcoder {

SubfileReader::SubfileReader(const std::string& fpath):
    curr_mgrec_pos(0),
    reader(fpath, std::ios::binary), // Start from the end to tell the filesize
    bitreader(reader),
    rec()
{}

SubfileReader::~SubfileReader(){
    reader.close();
}

bool SubfileReader::readRecord(){
//    std::cout << reader.tellg();
//    printf(std::to_string(reader.tellg()));
    auto pos = reader.tellg();
    if (reader.good() && reader.peek() != EOF){
        rec = genie::core::record::Record(bitreader);
        curr_mgrec_pos = getMinPos(rec);

        return true;
    } else {
        bitreader.flush();
        reader.close();
        return false;
    }
}

void SubfileReader::writeRecord(genie::util::BitWriter& bitwriter){
    rec.write(bitwriter);
    bitwriter.flush();
}

uint64_t SubfileReader::getPos() const{
    return curr_mgrec_pos;
}

uint64_t getMinPos(const genie::core::record::Record& r){

    auto alignments = r.getAlignments();

    UTILS_DIE_IF(alignments.empty(), "Unmapped record");

    uint64_t first_pos = alignments.front().getPosition();
    if (r.isRead1First()){
        return first_pos;
    } else{
        auto &split = r.getAlignments().front().getAlignmentSplits().front();
        UTILS_DIE_IF(split->getType() != genie::core::record::AlignmentSplit::Type::SAME_REC,
                     "Only same rec split alignments supported");
        return first_pos + dynamic_cast<const genie::core::record::alignment_split::SameRec &>(*split).getDelta();
    }
}

bool compare(const genie::core::record::Record &r1, const genie::core::record::Record &r2) {
    if (r1.getAlignments().empty()) {
        return false;
    }
    if (r2.getAlignments().empty()) {
        return true;
    }
    return getMinPos(r1) < getMinPos(r2);
}

}