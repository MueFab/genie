/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/read/spring/encoder.h"
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "genie/quality/paramqv1/qv_coding_config_1.h"
#include "genie/read/spring/call-template-functions.h"
#include "genie/read/spring/encoder-source.h"
#include "genie/read/spring/generate-read-streams.h"
#include "genie/read/spring/reorder-compress-quality-id.h"
#include "genie/util/thread-manager.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::spring {

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::flowIn(core::record::Chunk&& t, const util::Section& id) {
    preprocessor.preprocess(std::move(t), id);
    skipOut(id);
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::flushIn(uint64_t& pos) {
    if (!preprocessor.used) {
        flushOut(pos);
        return;
    }
    preprocessor.finish(pos);

    std::vector<core::parameter::EncodingSet> params;
    auto loc_cp = preprocessor.cp;
    util::Watch watch;
    core::stats::PerfStats stats = preprocessor.getStats();
#ifndef GENIE_USE_OPENMP
    loc_cp.num_thr = 1;
#endif

    watch.reset();
    std::cerr << "Reordering ...\n";
    call_reorder(preprocessor.temp_dir, loc_cp);
    std::cerr << "Reordering done!\n";
    stats.addDouble("time-spring-reorder", watch.check());

    watch.reset();
    std::cerr << "Encoding ...\n";
    call_encoder(preprocessor.temp_dir, loc_cp);
    std::cerr << "Encoding done!\n";
    stats.addDouble("time-spring-encoding", watch.check());

    watch.reset();
    std::cerr << "Generating read streams ...\n";
    generate_read_streams(preprocessor.temp_dir, loc_cp, entropycoder, params, stats, writeOutStreams);
    std::cerr << "Generating read streams done!\n";
    stats.addDouble("time-spring-gen-reads", watch.check());

    if (preprocessor.cp.preserve_quality || preprocessor.cp.preserve_id) {
        watch.reset();
        std::cerr << "Reordering and compressing quality and/or ids ...\n";
        reorder_compress_quality_id(preprocessor.temp_dir, loc_cp, qvcoder, namecoder, entropycoder, params, stats,
                                    writeOutStreams);
        std::cerr << "Reordering and compressing quality and/or ids done!\n";
        stats.addDouble("time-spring-qual-name", watch.check());
    }

    SpringSource src(this->preprocessor.temp_dir, this->preprocessor.cp, params, stats);
    src.setDrain(this->drain);
    std::vector<util::OriginalSource*> srcVec = {&src};
    util::ThreadManager mgr(preprocessor.cp.num_thr, pos);
    mgr.setSource(srcVec);
    mgr.run();

    std::filesystem::remove(preprocessor.temp_dir + "/blocks_id.bin");
    std::filesystem::remove(preprocessor.temp_dir + "/read_order.bin");
    std::filesystem::remove_all(preprocessor.temp_dir);

    preprocessor.setup(preprocessor.working_dir, preprocessor.cp.num_thr, preprocessor.cp.paired_end);

    flushOut(pos);
}

// ---------------------------------------------------------------------------------------------------------------------

Encoder::Encoder(const std::string& working_dir, size_t num_thr, bool paired_end, bool _write_raw)
    : ReadEncoder(_write_raw) {
    preprocessor.setup(working_dir, num_thr, paired_end);
}

// ---------------------------------------------------------------------------------------------------------------------

void Encoder::skipIn(const util::Section& id) {
    preprocessor.skip(id);
    skipOut(id);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::spring

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
