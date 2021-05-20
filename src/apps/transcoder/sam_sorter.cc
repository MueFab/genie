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
    close();
}

bool SubfileReader::readRecord(){
    if (good()){
        rec = genie::core::record::Record(bitreader);

        if (rec.getAlignments().empty()){
            curr_mgrec_pos = getMinPos(rec);
        }

        return true;
    } else {
        return false;
    }
}

genie::core::record::Record&& SubfileReader::moveRecord(){
    return std::move(rec);
}

const genie::core::record::Record& SubfileReader::getRecord() const{
    return rec;
}

void SubfileReader::writeRecord(genie::util::BitWriter& bitwriter){
    rec.write(bitwriter);
    bitwriter.flush();
}

uint64_t SubfileReader::getPos() const{
    return curr_mgrec_pos;
}

bool SubfileReader::good(){
    return reader.good() && reader.peek() != EOF;
}

void SubfileReader::close(){
    bitreader.flush();
    reader.close();
}

uint64_t getMinPos(const genie::core::record::Record& r){

    auto alignments = r.getAlignments();

    if (alignments.empty()){
        return 0;
    }

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