#include "access_unit.h"
#include "ureads-encoder/exceptions.h"

#include <sstream>
#include <gabac/data-block.h>

void Mm_cfg::write(BitWriter &writer) {
    writer.write(mm_threshold, 16);
    writer.write(mm_count, 32);
}

void Ref_cfg::write(BitWriter &writer) {
    writer.write(ref_sequence_ID, 16);
    writer.write(ref_start_position, 64); // TODO: Size
    writer.write(ref_end_position, 64); // TODO: Size
}

void Extended_AU::write(BitWriter &writer) {
    writer.write(extended_AU_start_position, 64); // TODO: Size
    writer.write(extended_AU_end_position, 64); // TODO: Size
}

void AU_Type_Cfg::write(BitWriter &writer) {
    writer.write(sequence_ID, 16);
    writer.write(AU_start_position, 64); // TODO: Size
    writer.write(AU_end_position, 64); // TODO: Size
    for (auto &i : extended_AU) {
        i.write(writer);
    }
}

Block::Block(uint8_t _descriptor_ID, std::vector<uint8_t> *_payload)
        : reserved(0),
          descriptor_ID(_descriptor_ID),
          reserved_2(0),
          block_payload_size(_payload->size()),
          payload(std::move(*_payload)) {
}

Block::Block()
        : reserved(0),
          descriptor_ID(0),
          reserved_2(0),
          block_payload_size(0),
          payload(0) {
}

void Block::write(BitWriter &writer) {
    writer.write(reserved, 1);
    writer.write(descriptor_ID, 7);
    writer.write(reserved_2, 3);
    writer.write(block_payload_size, 29);
    for (auto &i : payload) {
        writer.write(i, 8);
    }
    writer.flush();
}

Access_unit::Access_unit(
        uint32_t _access_unit_ID,
        uint8_t _parameter_set_ID,
        AU_type _au_type,
        uint32_t _reads_count,
        Dataset_type dataset_type
) : Data_unit(Data_unit_type::access_unit),
    reserved(0),
    data_unit_size(0),
    access_unit_ID(_access_unit_ID),
    num_blocks(0),
    parameter_set_ID(_parameter_set_ID),
    au_type(_au_type),
    reads_count(_reads_count),
    mm_cfg(0),
    ref_cfg(0),
    au_Type_U_Cfg(0),
    num_signatures(0),
    u_cluster_signatures(0),
    blocks(0),
    internalBitCounter(0) {
    if (au_type == AU_type::N_TYPE_AU || au_type == AU_type::M_TYPE_AU) {
        mm_cfg.emplace_back(); // TODO: Fill for types N and M
        GENIE_THROW_RUNTIME_EXCEPTION("Types N and M not supported");
    }
    if (dataset_type == Dataset_type::reference) {
        ref_cfg.emplace_back(); // TODO: Fill for dataset_type 2
        GENIE_THROW_RUNTIME_EXCEPTION("Dataset type reference not supported");
    }
    if (au_type != AU_type::U_TYPE_AU) {
        au_Type_U_Cfg.emplace_back(); // TODO: Fill for aligned data
        GENIE_THROW_RUNTIME_EXCEPTION("AU type != U not supported");
    } else {
/*        if (multiple_signature_base != 0) { // TODO: Check
            GENIE_THROW_RUNTIME_EXCEPTION("multiple_signature_base not supported");
        }*/
    }

    std::stringstream s;
    BitWriter bw(&s);
    write(&bw);

    internalBitCounter = bw.getBitsWritten();
    data_unit_size = internalBitCounter / 8;
    if (internalBitCounter % 8) {
        data_unit_size += 1;
    }
}

void Access_unit::write(BitWriter *writer) {
    Data_unit::write(writer);
    writer->write(reserved, 3);
    writer->write(data_unit_size, 29);
    writer->write(access_unit_ID, 32);
    writer->write(num_blocks, 8);
    writer->write(parameter_set_ID, 8);
    writer->write(uint8_t(au_type), 4);
    writer->write(reads_count, 32);
    for (auto &i : mm_cfg) {
        i.write(*writer);
    }
    for (auto &i : ref_cfg) {
        i.write(*writer);
    }
    for (auto &i : au_Type_U_Cfg) {
        i.write(*writer);
    }
    for (auto &i : num_signatures) {
        writer->write(i, 16);
    }
    for (auto &i : u_cluster_signatures) {
        writer->write(i, 64); // TODO: size
    }
    writer->flush(); // Zero bytes
    for (auto &i : blocks) {
        i->write(*writer);
    }
}

void Access_unit::addBlock(std::unique_ptr<Block> block) {
    data_unit_size += block->getTotalSize();
    ++num_blocks;
    blocks.push_back(std::move(block));
}

std::vector<uint8_t> create_payload(const std::vector<gabac::DataBlock> &block) {
    std::vector<uint8_t> ret;
    uint64_t totalSize = 0;
    for (auto &load : block) {
        totalSize += load.getRawSize();
        totalSize += sizeof(uint32_t);
    }
    if(block.size() > 0) {
        totalSize -= sizeof(uint32_t);
    }
    ret.reserve(totalSize);

    for (size_t i = 0; i < block.size(); ++i) {
        auto &load = block[i];
        if (i != block.size() - 1) {
            uint32_t size = load.getRawSize();
            ret.insert(ret.end(), reinterpret_cast<uint8_t *>(&size),
                       reinterpret_cast<uint8_t *>(&size) + sizeof(uint32_t));
        }
        if (load.getRawSize()) {
            ret.insert(ret.end(), reinterpret_cast<const uint8_t *>(load.getData()),
                       reinterpret_cast<const uint8_t *>(load.getData()) + load.getRawSize());
        }
    }
    return ret;
}

Access_unit create_quick_access_unit(uint32_t access_unit_id, uint8_t parameter_set_id, uint32_t reads_count,
                                     std::vector<std::vector<gabac::DataBlock>> *data) {
    Access_unit au(access_unit_id, parameter_set_id, Data_unit::AU_type::U_TYPE_AU, reads_count,
                   Data_unit::Dataset_type::non_aligned);
    for (size_t i = 0; i < data->size(); ++i) {
        if(i == 11 || i == 15 || data->at(i).empty()) {
            continue; // TODO: Token types
        }
        auto &desc = (*data)[i];
        auto payload = create_payload(desc);
        au.addBlock(std::unique_ptr<Block>(new Block(i, &payload)));
    }
    return au;
}
