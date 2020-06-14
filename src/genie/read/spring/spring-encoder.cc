/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "spring-encoder.h"
#include <genie/quality/paramqv1/qv_coding_config_1.h>
#include <genie/util/thread-manager.h>
#include <cmath>
#include <utility>
#include "generate-read-streams.h"
#include "reorder-compress-quality-id.h"

namespace genie {
namespace read {
namespace spring {

void SpringEncoder::flowIn(core::record::Chunk&& t, const util::Section& id) {
    preprocessor.preprocess(std::move(t), id);
    skipOut(id);
}

class SpringSource : public util::OriginalSource, public util::Source<core::AccessUnit> {
   private:
    compression_params cp;
    std::string temp_dir;
    uint32_t num_AUs;
    util::SideSelector<genie::core::QVEncoder, genie::core::QVEncoder::QVCoded, const genie::core::record::Chunk&>*
        coder;
    util::SideSelector<genie::core::NameEncoder, genie::core::AccessUnit::Descriptor,
                       const genie::core::record::Chunk&>* ncoder;

   public:
    SpringSource(core::ReadEncoder::QvSelector* _coder,
                 core::ReadEncoder::NameSelector* _ncoder,
                 const compression_params& _cp, std::string _temp_dir)
        : cp(_cp), temp_dir(std::move(_temp_dir)), coder(_coder), ncoder(_ncoder) {
    }

    bool pump(size_t& id, std::mutex&) override {

    }

    void flushIn(size_t& pos) override { flushOut(pos); }
};

void SpringEncoder::flushIn(size_t& pos) {
    preprocessor.finish(pos);

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

    std::cout << "Generating read streams ...\n";
    auto grs_start = std::chrono::steady_clock::now();
    generate_read_streams(preprocessor.temp_dir, preprocessor.cp, entropycoder);
    auto grs_end = std::chrono::steady_clock::now();
    std::cout << "Generating read streams done!\n";
    std::cout << "Time for this step: "
              << std::chrono::duration_cast<std::chrono::seconds>(grs_end - grs_start).count() << " s\n";

    if (preprocessor.cp.preserve_quality || preprocessor.cp.preserve_id) {
        std::cout << "Reordering and compressing quality and/or ids ...\n";
        auto rcqi_start = std::chrono::steady_clock::now();
        reorder_compress_quality_id(preprocessor.temp_dir, preprocessor.cp, qvcoder, namecoder, entropycoder);
        auto rcqi_end = std::chrono::steady_clock::now();
        std::cout << "Reordering and compressing quality and/or ids done!\n";
        std::cout << "Time for this step: "
                  <<
                  std::chrono::duration_cast<std::chrono::seconds>(rcqi_end -
                                                                   rcqi_start).count() << " s\n";
    }

   /* SpringSource src(this->qvcoder, this->namecoder, this->preprocessor.cp, this->preprocessor.temp_dir);
    src.setDrain(this->drain);
    std::vector<util::OriginalSource*> srcVec = {&src};
    util::ThreadManager mgr(preprocessor.cp.num_thr);
    mgr.setSource(srcVec);
    mgr.run();*/
}
}  // namespace spring
}  // namespace read
}  // namespace genie
