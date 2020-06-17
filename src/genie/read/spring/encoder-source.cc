/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "encoder-source.h"
#include <filesystem/filesystem.hpp>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

// ---------------------------------------------------------------------------------------------------------------------

SpringSource::SpringSource(const std::string& temp_dir, const compression_params& cp,
                           std::vector<core::parameter::ParameterSet>& p, core::stats::PerfStats s)
    : params(p), stats(std::move(s)) {
    auId = 0;
    // read info about number of blocks (AUs) and the number of reads and records in those
    const std::string block_info_file = temp_dir + "/block_info.bin";
    std::ifstream f_block_info(block_info_file, std::ios::binary);
    f_block_info.read((char*)&num_AUs, sizeof(uint32_t));
    num_reads_per_AU = std::vector<uint32_t>(num_AUs);
    num_records_per_AU = std::vector<uint32_t>(num_AUs);
    f_block_info.read((char*)&num_reads_per_AU[0], num_AUs * sizeof(uint32_t));
    if (!cp.paired_end)  // num reads same as num records per AU
        num_records_per_AU = num_reads_per_AU;
    else
        f_block_info.read((char*)&num_records_per_AU[0], num_AUs * sizeof(uint32_t));

    f_block_info.close();
    ghc::filesystem::remove(block_info_file);

    // define descriptors corresponding to reads, ids and quality (so as to read them from file)
    read_desc_prefix = temp_dir + "/read_streams.";
    id_desc_prefix = temp_dir + "/id_streams.";
    quality_desc_prefix = temp_dir + "/quality_streams.";
}

// ---------------------------------------------------------------------------------------------------------------------

bool SpringSource::pump(size_t& id, std::mutex& lock) {
    core::AccessUnit au(core::parameter::ParameterSet(), 0);
    util::Section sec{};
    {
        std::unique_lock<std::mutex> guard(lock);

        if (auId >= num_AUs) {
            return false;
        }
        au = core::AccessUnit(std::move(params[auId]), num_reads_per_AU[auId]);
        au.setClassType(core::record::ClassType::CLASS_U);

        for (auto& d : au) {
            std::string filename;
            size_t count = core::getDescriptor(d.getID()).subseqs.size();
            if (d.getID() == core::GenDesc::RNAME) {
                filename = id_desc_prefix + std::to_string(auId);
            } else if (d.getID() == core::GenDesc::QV) {
                filename = quality_desc_prefix + std::to_string(auId);
                count = au.getParameters().getQVConfig(core::record::ClassType::CLASS_U).getNumSubsequences();
            } else {
                filename = read_desc_prefix + std::to_string(auId) + "." + std::to_string(uint8_t(d.getID()));
            }
            if (!ghc::filesystem::exists(filename) || !ghc::filesystem::file_size(filename)) {
                continue;
            }
            std::ifstream input(filename);
            util::BitReader br(input);
            d = core::AccessUnit::Descriptor(d.getID(), count, ghc::filesystem::file_size(filename), br);
            input.close();
            ghc::filesystem::remove(filename);
        }
        auId++;
        sec = {id, au.getNumReads(), true};
        id += sec.length;
    }
    au.setStats(std::move(stats));
    stats = core::stats::PerfStats();
    flowOut(std::move(au), sec);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

void SpringSource::flushIn(size_t& pos) { flushOut(pos); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------