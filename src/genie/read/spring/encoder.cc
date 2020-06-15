/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "encoder.h"
#include <genie/quality/paramqv1/qv_coding_config_1.h>
#include <genie/util/thread-manager.h>
#include <cmath>
#include <utility>
#include "call-template-functions.h"
#include "generate-read-streams.h"
#include "reorder-compress-quality-id.h"

namespace genie {
namespace read {
namespace spring {

void Encoder::flowIn(core::record::Chunk&& t, const util::Section& id) {
    preprocessor.preprocess(std::move(t), id);
    skipOut(id);
}

class SpringSource : public util::OriginalSource, public util::Source<core::AccessUnit> {
   private:
    uint32_t num_AUs;
    std::string read_desc_prefix;
    std::string id_desc_prefix;
    std::string quality_desc_prefix;
    std::vector<uint32_t> num_records_per_AU;
    std::vector<uint32_t> num_reads_per_AU;
    uint32_t auId;
    std::vector<core::parameter::ParameterSet>& params;

   public:
    SpringSource(const std::string& temp_dir, const compression_params& cp, std::vector<core::parameter::ParameterSet>& p) : params(p) {
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

    bool pump(size_t& id, std::mutex& lock) override {
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
        flowOut(std::move(au), sec);
        return true;
    }

    void flushIn(size_t& pos) override { flushOut(pos); }
};

void Encoder::flushIn(size_t& pos) {
    preprocessor.finish(pos);


    std::vector<core::parameter::ParameterSet> params;
    auto loc_cp = preprocessor.cp;
#ifndef GENIE_USE_OPENMP
    loc_cp.num_thr = 1;
#endif

    std::cout << "Reordering ...\n";
    auto reorder_start = std::chrono::steady_clock::now();
    call_reorder(preprocessor.temp_dir, loc_cp);
    auto reorder_end = std::chrono::steady_clock::now();
    std::cout << "Reordering done!\n";
    std::cout << "Time for this step: "
              << std::chrono::duration_cast<std::chrono::seconds>(reorder_end - reorder_start).count() << " s\n";

    std::cout << "Encoding ...\n";
    auto encoder_start = std::chrono::steady_clock::now();
    call_encoder(preprocessor.temp_dir, loc_cp);
    auto encoder_end = std::chrono::steady_clock::now();
    std::cout << "Encoding done!\n";
    std::cout << "Time for this step: "
              << std::chrono::duration_cast<std::chrono::seconds>(encoder_end - encoder_start).count() << " s\n";

    std::cout << "Generating read streams ...\n";
    auto grs_start = std::chrono::steady_clock::now();
    generate_read_streams(preprocessor.temp_dir, loc_cp, entropycoder, params);
    auto grs_end = std::chrono::steady_clock::now();
    std::cout << "Generating read streams done!\n";
    std::cout << "Time for this step: " << std::chrono::duration_cast<std::chrono::seconds>(grs_end - grs_start).count()
              << " s\n";

    if (preprocessor.cp.preserve_quality || preprocessor.cp.preserve_id) {
        std::cout << "Reordering and compressing quality and/or ids ...\n";
        auto rcqi_start = std::chrono::steady_clock::now();
        reorder_compress_quality_id(preprocessor.temp_dir, loc_cp, qvcoder, namecoder, entropycoder, params);
        auto rcqi_end = std::chrono::steady_clock::now();
        std::cout << "Reordering and compressing quality and/or ids done!\n";
        std::cout << "Time for this step: "
                  << std::chrono::duration_cast<std::chrono::seconds>(rcqi_end - rcqi_start).count() << " s\n";
    }

    SpringSource src(this->preprocessor.temp_dir, this->preprocessor.cp, params);
    src.setDrain(this->drain);
    std::vector<util::OriginalSource*> srcVec = {&src};
    util::ThreadManager mgr(preprocessor.cp.num_thr, pos);
    mgr.setSource(srcVec);
    mgr.run();

    flushOut(pos);
}
}  // namespace spring
}  // namespace read
}  // namespace genie
