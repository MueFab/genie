/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "spring-encoder.h"
#include <genie/util/thread-manager.h>
#include "generate-read-streams.h"
#include "reorder-compress-quality-id.h"

namespace genie {
namespace read {
namespace spring {

void SpringEncoder::flowIn(core::record::Chunk&& t, size_t id) { preprocessor.preprocess(std::move(t), id); }

class SpringSource : public util::OriginalSource, public util::Source<core::AccessUnitRaw> {
   private:
    se_data data;
    pe_block_data bdata;
    compression_params cp;
    std::string temp_dir;
    util::FastqStats* stats;
    uint32_t num_AUs;
    std::vector<uint32_t> num_reads_per_AU;
    std::vector<uint32_t> num_records_per_AU;

   public:
    SpringSource(const compression_params& _cp, const std::string& _temp_dir, util::FastqStats* _stats)
        : cp(_cp), temp_dir(_temp_dir), stats(_stats) {
        if (cp.paired_end) {
            loadPE_Data(cp, temp_dir, true, &data);
            generateBlocksPE(data, &bdata);
            data.order_arr.clear();
        } else {
            loadSE_Data(cp, temp_dir, &data);
        }
        const std::string block_info_file = temp_dir + "/block_info.bin";
        std::ifstream f_block_info(block_info_file, std::ios::binary);

        f_block_info.read((char*)&num_AUs, sizeof(uint32_t));
        num_reads_per_AU = std::vector<uint32_t>(num_AUs);
        num_records_per_AU = std::vector<uint32_t>(num_AUs);
        f_block_info.read((char*)&num_reads_per_AU[0], num_AUs*sizeof(uint32_t));
        if (!cp.paired_end) // num reads same as num records per AU
            num_records_per_AU = num_reads_per_AU;
        else
            f_block_info.read((char*)&num_records_per_AU[0], num_AUs*sizeof(uint32_t));
    }

    bool pump(size_t id) override {
        core::parameter::ParameterSet ps;
        core::AccessUnitRaw au(std::move(ps), 0);
        util::FastqStats stats;
        if (cp.paired_end) {
            au = generate_read_streams_pe(data, bdata, id, num_records_per_AU[id]);
        } else {
            au = generate_read_streams_se(data, id, num_records_per_AU[id]);
        }


        flowOut(std::move(au), id);
        return id != num_AUs - 1 ;
    }

    /**
     * @brief Signal end of data.
     */
    virtual void dryIn() override { dryOut(); }
};

void SpringEncoder::dryIn() {
    preprocessor.finish();

    util::FastqStats stats;

    std::cout << "Reordering ...\n";
    auto reorder_start = std::chrono::steady_clock::now();
    call_reorder(preprocessor.temp_dir, preprocessor.cp);
    auto reorder_end = std::chrono::steady_clock::now();
    std::cout << "Reordering done!\n";
    std::cout << "Time for this step: "
              << std::chrono::duration_cast<std::chrono::seconds>(reorder_end - reorder_start).count() << " s\n";

    std::cout << "Encoding ...\n";
    auto encoder_start = std::chrono::steady_clock::now();
    call_encoder(preprocessor.temp_dir, preprocessor.cp);
    auto encoder_end = std::chrono::steady_clock::now();
    std::cout << "Encoding done!\n";
    std::cout << "Time for this step: "
              << std::chrono::duration_cast<std::chrono::seconds>(encoder_end - encoder_start).count() << " s\n";

    if (preprocessor.cp.preserve_quality || preprocessor.cp.preserve_id) {
        std::cout << "Reordering and compressing quality and/or ids ...\n";
        auto rcqi_start = std::chrono::steady_clock::now();
        reorder_compress_quality_id(preprocessor.temp_dir, preprocessor.cp, &stats);
        auto rcqi_end = std::chrono::steady_clock::now();
        std::cout << "Reordering and compressing quality and/or ids done!\n";
        std::cout << "Time for this step: "
                  << std::chrono::duration_cast<std::chrono::seconds>(rcqi_end - rcqi_start).count() << " s\n";
    }

    SpringSource src(this->preprocessor.cp, this->preprocessor.temp_dir, this->stats);
    src.setDrain(this->drain);

    util::ThreadManager mgr(preprocessor.cp.num_thr, &src);
    mgr.run();
}
}  // namespace spring
}  // namespace read
}  // namespace genie