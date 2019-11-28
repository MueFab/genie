#ifndef GENIE_MPEGG_RECORD_H
#define GENIE_MPEGG_RECORD_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <util/exceptions.h>
#include "format/part2/data_unit.h"
#include "format/part2/make_unique.h"

class SplitAlignmentSameRec : public SplitAlignment {
    int64_t delta : 48;
    std::unique_ptr<Alignment> alignment;

public:
    SplitAlignmentSameRec(int64_t _delta,
                          std::unique_ptr<Alignment> _alignment
    ) : SplitAlignment(SplitAlignment::SplitAlignmentType::SAME_REC),
        delta(_delta),
        alignment(std::move(_alignment)) {

    }
};

class SplitAlignmentOtherRec : public SplitAlignment {
    uint64_t split_pos : 40;
    uint16_t split_seq_ID : 16;

public:
    SplitAlignmentOtherRec(
            uint64_t _split_pos,
            uint16_t _split_seq_ID
    ) : SplitAlignment(SplitAlignment::SplitAlignmentType::OTHER_REC),
        split_pos(_split_pos),
        split_seq_ID(_split_seq_ID) {

    }
};

class SplitAlignmentUnpaired : public SplitAlignment {
public:
    SplitAlignmentUnpaired() : SplitAlignment(SplitAlignment::SplitAlignmentType::UNPAIRED) {

    }
};

class MpeggRecord {
    uint8_t number_of_template_segments : 8;
    //uint8_t number_of_record_segments : 8;
    //uint16_t number_of_alignments : 16;
    format::DataUnit::AuType class_ID : 8;
    //uint8_t read_group_len : 8;
    uint8_t read_1_first : 8;
    // if(number_of_alignments > 0)
    std::unique_ptr<MetaAlignment> sharedAlignmentInfo;

    //  for (rSeg=0; rSeg < number_of_record_segments; rSeg++)
    // std::vector<uint32_t> read_len; //: 24;

    uint8_t qv_depth : 8;
    //uint8_t read_name_len : 8;
    std::unique_ptr<std::string> read_name;
    std::unique_ptr<std::string> read_group;
    // for (rSeg=0; rSeg < number_of_record_segments; rSeg++)
    std::vector<std::unique_ptr<Segment>> reads;

    // for (noa=0; noa < number_of_alignments; noa++)
    std::vector<std::unique_ptr<AlignmentContainer>> alignmentInfo;
    uint8_t flags : 8;

    std::unique_ptr<ExternalAlignment> moreAlignmentInfo;

    MpeggRecord(
    ) : number_of_template_segments(0),
        class_ID(format::DataUnit::AuType::NONE),
        read_1_first(0),
        sharedAlignmentInfo(nullptr),
        qv_depth(0),
        read_name(nullptr),
        read_group(nullptr),
        reads(0),
        alignmentInfo(0),
        flags(0),
        moreAlignmentInfo(nullptr) {

    }

public:
    MpeggRecord(
            uint8_t _number_of_template_segments,
            format::DataUnit::AuType _auType,
            std::unique_ptr<std::string> _read_name,
            std::unique_ptr<std::string> _read_group,
            uint8_t _flags
    ) : number_of_template_segments(_number_of_template_segments),
        class_ID(_auType),
        read_1_first(true),
        sharedAlignmentInfo(nullptr),
        qv_depth(0),
        read_name(std::move(_read_name)),
        read_group(std::move(_read_group)),
        reads(0),
        alignmentInfo(0),
        flags(_flags),
        moreAlignmentInfo(format::make_unique<ExternalAlignmentNone>()) {

    }

    explicit MpeggRecord(util::BitReader *reader) {
        number_of_template_segments = reader->read(8);
        reads.resize(reader->read(8));
        alignmentInfo.resize(reader->read(16));
        class_ID = format::DataUnit::AuType (reader->read(8));
        read_group->resize(reader->read(8));
        read_1_first = reader->read(8);
        if(!alignmentInfo.empty()) {
            sharedAlignmentInfo = format::make_unique<MetaAlignment>(reader);
        }
        std::vector<uint32_t> readSizes(reads.size());
        for(auto& a : readSizes) {
            a = reader->read(24);
        }
        qv_depth = reader->read(8);
        read_name->resize(reader->read(8));
        for(auto& c : *read_name) {
            c = reader->read(8);
        }
        for(auto& c : *read_group) {
            c = reader->read(8);
        }

        size_t index = 0;
        for(auto& r : reads) {
            r = format::make_unique<Segment>(readSizes[index], reader);
            ++index;
        }
        for(auto& a : alignmentInfo) {
            a = format::make_unique<AlignmentContainer>(reader);
        }
        flags = reader->read(8);
        moreAlignmentInfo = ExternalAlignment::factory(reader);
    }

    void addRecordSegment(std::unique_ptr<Segment> rec) {
        if(reads.size() == number_of_template_segments) {
            UTILS_DIE("Record already full");
        }
        if(!reads.empty() && rec->getQvDepth() != qv_depth) {
            UTILS_DIE("Incompatible qv depth");
        }
        reads.push_back(std::move(rec));
    }

    void addAlignment(uint16_t _seq_id, std::unique_ptr<AlignmentContainer> rec) {
        if(sharedAlignmentInfo) {
            if(rec->getAsDepth() != sharedAlignmentInfo->getAsDepth()) {
                UTILS_DIE("Incompatible as depth");
            }
            if(_seq_id != sharedAlignmentInfo->getSeqID()) {
                UTILS_DIE("Incompatible seq id");
            }
        }
        else {
            sharedAlignmentInfo = format::make_unique<MetaAlignment>(_seq_id, rec->getAsDepth());
        }
        alignmentInfo.push_back(std::move(rec));
    }

    virtual void write(util::BitWriter *writer) const {
        writer->write(number_of_template_segments, 8);
        writer->write(reads.size(), 8);
        writer->write(alignmentInfo.size(), 16);
        writer->write((uint8_t) class_ID, 8);
        writer->write(read_group->length(), 8);
        writer->write(read_1_first, 8);
        if(sharedAlignmentInfo) {
            sharedAlignmentInfo->write(writer);
        }
        for(const auto& a : reads) {
            writer->write(a->getLength(), 24);
        }
        writer->write(qv_depth, 8);
        writer->write(read_name->length(), 8);
        for(const auto& c : *read_name) {
            writer->write(c, 8);
        }
        for(const auto& c : *read_group) {
            writer->write(c, 8);
        }
        for(const auto& r : reads) {
            r->write(writer);
        }
        for(const auto& a : alignmentInfo) {
            a->write(writer);
        }
        writer->write(flags, 8);
        moreAlignmentInfo->write(writer);
    }

    std::unique_ptr<MpeggRecord> clone() const {
        std::unique_ptr<MpeggRecord> ret;

        return ret;
    }
};


#endif //GENIE_MPEGG_RECORD_H
