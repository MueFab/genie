#include "mpegg-record.h"


std::unique_ptr<ExternalAlignment> ExternalAlignment::factory(util::BitReader *reader) {
    auto type = MoreAlignmentInfoType(reader->read(8));
    switch(type){
        case MoreAlignmentInfoType::NONE:
            return format::make_unique<ExternalAlignmentNone>(reader);
        case MoreAlignmentInfoType::OTHER_REC:
            return format::make_unique<ExternalAlignmentOtherRec>(reader);
        default:
            UTILS_DIE("Unknown MoreAlignmentInfoType");
    }
}