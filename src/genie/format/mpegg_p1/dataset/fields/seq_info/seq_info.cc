#include "genie/util/runtime-exception.h"
#include "seq_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

SequenceConfig::SequenceConfig()
    : reference_ID(0),
      seq_IDs(),
      seq_blocks(),
      thress() {}

SequenceConfig::SequenceConfig(uint8_t _ref_ID)
    : reference_ID(_ref_ID),
      seq_IDs(),
      seq_blocks(),
      thress() {}

void SequenceConfig::ReadSequenceConfig(util::BitReader& reader) {

//    size_t start_pos = reader.getPos();

    // seq_count u(16)
    reader.read<uint16_t>();

    if (anySeq()){
        // reference_ID u(8)
        reference_ID = reader.read<uint8_t>();

        // seq_ID u(16)
        for (auto seq = 0; seq < getSeqCount(); seq++){
            reader.read<uint16_t>();
        }

        // seq_blocks u(32)
        for (auto seq = 0; seq < getSeqCount(); seq++){
            reader.read<uint32_t>();
        }
        // tflag[0] u(1)
        reader.read<bool>(1);

        // thres[0] u(31)
        reader.read<uint32_t>(31);

        auto thres = thress.begin();

        while (thres != thress.end()){
            auto next_thres = thres +1;

            bool tflag = *thres != *next_thres;
            // tflag[i] u(1)
            reader.read<bool>(1);

            if (tflag){
                // thres[i] u(31)
                reader.read<uint32_t>(31);
            }

            thres = next_thres;
        }
    }

//    UTILS_DIE_IF(reader.getPos()-start_pos != length, "Invalid readUAccessUnitInfo length!");
}

bool SequenceConfig::isValid() const { return seq_IDs.size() == thress.size(); }

bool SequenceConfig::anySeq() const { return !seq_IDs.empty(); }

void SequenceConfig::setRefID(uint8_t _ref_ID) {
    reference_ID = _ref_ID;
}

void SequenceConfig::addEntry(uint16_t seq_ID, uint32_t seq_block) {
    seq_IDs.push_back(seq_ID);
    seq_blocks.push_back(seq_block);
}

void SequenceConfig::addEntries(std::vector<uint16_t>& _seq_IDs, std::vector<uint32_t>& _seq_blocks) {

    UTILS_DIE_IF(_seq_IDs.empty() || _seq_blocks.empty(),
                 "Either _seq_IDs or _seq_blocks is empty");

    UTILS_DIE_IF(_seq_IDs.size() != _seq_blocks.size(),
                 "The number of _seq_IDs and _seq_blocks are different!");

    std::move(_seq_IDs.begin(), _seq_IDs.end(), std::back_inserter(seq_IDs));
    std::move(_seq_blocks.begin(), _seq_blocks.end(), std::back_inserter(seq_blocks));
}

uint16_t SequenceConfig::getSeqCount() const { return seq_IDs.size(); }

const std::vector<uint16_t>& SequenceConfig::getSeqBlocks() const { return seq_blocks; }

uint64_t SequenceConfig::getBitLength() const {
    // seq_count u(16)
    uint64_t bitlen = 16;

    if (anySeq()){
        // reference_ID u(8)
        bitlen += 8;

        // seq_ID[] u(16)
        bitlen += 16 * getSeqCount();

        // seq_blocks[] u(32)
        bitlen += 32 * getSeqCount();

        // tflag[0] u(1)
        bitlen += 1;

        // thres[0] u(31)
        bitlen += 31;

        auto thres = thress.begin();

        while (thres != thress.end()){
            auto next_thres = thres +1;

            bool tflag = *thres != *next_thres;

            bitlen += 1;

            if (tflag){
                // thres[i] u(31)
                bitlen += 31;
            }

            thres = next_thres;
        }
    }

    return bitlen;
}

void SequenceConfig::write(util::BitWriter& writer) const {

    UTILS_DIE_IF(!isValid(),
                 "seq_count and size of thres[] are different!");

    // seq_count u(16)
    writer.write(getSeqCount(), 16);

    if (anySeq()){
        // reference_ID u(8)
        writer.write(reference_ID, 8);

        // seq_ID u(16)
        for (auto seq_ID: seq_IDs){
            writer.write(seq_ID, 16);
        }

        // seq_blocks u(32)
        for (auto seq_block: seq_blocks){
            writer.write(seq_block, 32);
        }
    }
}

void SequenceConfig::writeThres(util::BitWriter& bit_writer) const {

    UTILS_DIE_IF(!isValid(),
                 "seq_count and size of thres[] are different!");

    if (anySeq()) {

        auto thres = thress.begin();

        // tflag[0] u(1)
        bit_writer.write(0, 1);

        // thres[0] u(31)
        bit_writer.write(*thres, 31);

        while (thres != thress.end()){
            auto next_thres = thres +1;

            bool tflag = *thres != *next_thres;

            // tflag[i] u(1)
            bit_writer.write(tflag, 1);

            if (tflag){
                // thres[i] u(31)
                bit_writer.write(*next_thres, 31);
            }

            thres = next_thres;
        }

    }
}


}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie