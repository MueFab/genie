/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "encoder.h"
#include <genie/quality/paramqv1/qv_coding_config_1.h>
#include <genie/util/thread-manager.h>
#include <genie/util/watch.h>
#include <iostream>
#include <utility>
#include "call-template-functions.h"
#include "encoder-source.h"
#include "generate-read-streams.h"
#include "reorder-compress-quality-id.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::flowIn(core::record::Chunk&& t, const util::Section& id) {
    preprocessor.preprocess(std::move(t), id);
    skipOut(id);
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::flushIn(size_t& pos) {
    if (!preprocessor.used) {
        flushOut(pos);
        return;
    }
    preprocessor.finish(pos);

    std::vector<core::parameter::ParameterSet> params;
    auto loc_cp = preprocessor.cp;
    util::Watch watch;
    core::stats::PerfStats stats = preprocessor.getStats();
#ifndef GENIE_USE_OPENMP
    loc_cp.num_thr = 1;
#endif

    watch.reset();
    std::cout << "Reordering ...\n";
    call_reorder(preprocessor.temp_dir, loc_cp);
    std::cout << "Reordering done!\n";
    stats.addDouble("time-spring-reorder", watch.check());

    watch.reset();
    std::cout << "Encoding ...\n";
    call_encoder(preprocessor.temp_dir, loc_cp);
    std::cout << "Encoding done!\n";
    stats.addDouble("time-spring-encoding", watch.check());

    watch.reset();
    std::cout << "Generating read streams ...\n";
    generate_read_streams(preprocessor.temp_dir, loc_cp, entropycoder, params, stats);
    std::cout << "Generating read streams done!\n";
    stats.addDouble("time-spring-gen-reads", watch.check());

    if (preprocessor.cp.preserve_quality || preprocessor.cp.preserve_id) {
        watch.reset();
        std::cout << "Reordering and compressing quality and/or ids ...\n";
        reorder_compress_quality_id(preprocessor.temp_dir, loc_cp, qvcoder, namecoder, entropycoder, params, stats);
        std::cout << "Reordering and compressing quality and/or ids done!\n";
        stats.addDouble("time-spring-qual-name", watch.check());
    }

    SpringSource src(this->preprocessor.temp_dir, this->preprocessor.cp, params, stats);
    src.setDrain(this->drain);
    std::vector<util::OriginalSource*> srcVec = {&src};
    util::ThreadManager mgr(preprocessor.cp.num_thr, pos);
    mgr.setSource(srcVec);
    mgr.run();

    ghc::filesystem::remove(preprocessor.temp_dir + "/blocks_id.bin");
    ghc::filesystem::remove(preprocessor.temp_dir + "/read_order.bin");
    ghc::filesystem::remove(preprocessor.temp_dir);

    preprocessor.setup(preprocessor.working_dir, preprocessor.cp.num_thr, preprocessor.cp.paired_end);

    flushOut(pos);
}

// ---------------------------------------------------------------------------------------------------------------------

Encoder::Encoder(const std::string& working_dir, size_t num_thr, bool paired_end) {
    preprocessor.setup(working_dir, num_thr, paired_end);
}

// ---------------------------------------------------------------------------------------------------------------------

void flowIn(genie::core::record::Chunk&& t, const util::Section& id);

// ---------------------------------------------------------------------------------------------------------------------

void flushIn(size_t& pos);

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::skipIn(const util::Section& id) {
    preprocessor.skip(id);
    skipOut(id);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------