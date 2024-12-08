/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/spring/encoder.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "genie/quality/paramqv1/qv_coding_config_1.h"
#include "genie/read/spring/call_template_functions.h"
#include "genie/read/spring/encoder_source.h"
#include "genie/read/spring/generate_read_streams.h"
#include "genie/read/spring/reorder_compress_quality_id.h"
#include "genie/util/stop_watch.h"
#include "genie/util/thread_manager.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

// -----------------------------------------------------------------------------
void Encoder::FlowIn(core::record::Chunk&& t, const util::Section& id) {
  preprocessor_.Preprocess(std::move(t), id);
  SkipOut(id);
}

// -----------------------------------------------------------------------------
void Encoder::FlushIn(uint64_t& pos) {
  if (!preprocessor_.used) {
    FlushOut(pos);
    return;
  }
  preprocessor_.Finish(pos);

  std::vector<core::parameter::EncodingSet> params;
  auto loc_cp = preprocessor_.cp;
  util::Watch watch;
  core::stats::PerfStats stats = preprocessor_.GetStats();
#ifndef GENIE_USE_OPENMP
  loc_cp.num_thr = 1;
#endif

  watch.Reset();
  std::cerr << "Reordering ...\n";
  CallReorder(preprocessor_.temp_dir, loc_cp);
  std::cerr << "Reordering done!\n";
  stats.AddDouble("time-spring-reorder", watch.Check());

  watch.Reset();
  std::cerr << "Encoding ...\n";
  CallEncoder(preprocessor_.temp_dir, loc_cp);
  std::cerr << "Encoding done!\n";
  stats.AddDouble("time-spring-encoding", watch.Check());

  watch.Reset();
  std::cerr << "Generating read streams ...\n";
  GenerateReadStreams(preprocessor_.temp_dir, loc_cp, entropycoder_, params,
                      stats, write_out_streams_);
  std::cerr << "Generating read streams done!\n";
  stats.AddDouble("time-spring-gen-reads", watch.Check());

  if (preprocessor_.cp.preserve_quality || preprocessor_.cp.preserve_id) {
    watch.Reset();
    std::cerr << "Reordering and compressing quality and/or ids ...\n";
    ReorderCompressQualityId(preprocessor_.temp_dir, loc_cp, qvcoder_,
                             namecoder_, entropycoder_, params, stats,
                             write_out_streams_);
    std::cerr << "Reordering and compressing quality and/or ids done!\n";
    stats.AddDouble("time-spring-quality-name", watch.Check());
  }

  SpringSource src(this->preprocessor_.temp_dir, this->preprocessor_.cp, params,
                   stats);
  src.SetDrain(this->drain_);
  std::vector<util::OriginalSource*> src_vec = {&src};
  util::ThreadManager mgr(preprocessor_.cp.num_thr, pos);
  mgr.SetSource(src_vec);
  mgr.Run();

  std::filesystem::remove(preprocessor_.temp_dir + "/blocks_id.bin");
  std::filesystem::remove(preprocessor_.temp_dir + "/read_order.bin");
  std::filesystem::remove_all(preprocessor_.temp_dir);

  preprocessor_.Setup(preprocessor_.working_dir, preprocessor_.cp.num_thr,
                      preprocessor_.cp.paired_end);

  FlushOut(pos);
}

// -----------------------------------------------------------------------------
Encoder::Encoder(const std::string& working_dir, size_t num_thr,
                 bool paired_end, bool write_raw)
    : ReadEncoder(write_raw) {
  preprocessor_.Setup(working_dir, num_thr, paired_end);
}

// -----------------------------------------------------------------------------
void Encoder::SkipIn(const util::Section& id) {
  preprocessor_.Skip(id);
  SkipOut(id);
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
