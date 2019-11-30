#ifndef GENIE_MPEGG_RECORD_H
#define GENIE_MPEGG_RECORD_H

#include <cstdint>
#include <memory>
#include <vector>

namespace util {
    class BitWriter;
    class BitReader;
}

namespace format {
    namespace mpegg_rec {
        class MetaAlignment;
        class Segment;
        class AlignmentContainer;
        class ExternalAlignment;
        class MpeggRecord {
        public:
            enum class ClassType : uint8_t {
                NONE = 0,
                CLASS_P = 1,
                CLASS_N = 2,
                CLASS_M = 3,
                CLASS_I = 4,
                CLASS_HM = 5,
                CLASS_U = 6
            };
        private:
            uint8_t number_of_template_segments : 8;
            //uint8_t number_of_record_segments : 8;
            //uint16_t number_of_alignments : 16;
            ClassType class_ID;
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
        public:
            MpeggRecord();
            MpeggRecord(
                    uint8_t _number_of_template_segments,
                    ClassType _auTypeCfg,
                    std::unique_ptr<std::string> _read_name,
                    std::unique_ptr<std::string> _read_group,
                    uint8_t _flags
            );

            explicit MpeggRecord(util::BitReader *reader);

            void addRecordSegment(std::unique_ptr<Segment> rec);

            void addAlignment(uint16_t _seq_id, std::unique_ptr<AlignmentContainer> rec);

            const Segment* getRecordSegment(size_t index) const;

            size_t getNumberOfRecords() const;

            size_t getNumberOfAlignments() const;

            const AlignmentContainer* getAlignment(size_t index) const;

            virtual void write(util::BitWriter *writer) const;

            std::unique_ptr<MpeggRecord> clone() const;

            uint8_t getFlags() const;

            ClassType getClassID() const;
        };

        typedef std::vector<std::unique_ptr<MpeggRecord>> MpeggChunk;
    }
}



#endif //GENIE_MPEGG_RECORD_H
