/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.cc
 * @brief Implementation of the Encoder class for the Spring module.
 *
 * This file contains the implementation of the Encoder class, which manages
 * the encoding process for read sequences in the Spring module. It handles
 * preprocessing, organizing records into chunks, and managing the multithreaded
 * flow of data during the encoding process.
 *
 * @details The Encoder class utilizes the Preprocessor class to preprocess
 * input records and generates the final encoded access units that can be stored
 * or further processed. It includes functions for setting up file paths,
 * processing read segments and records, and managing temporary files and
 * directories.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
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
#include "genie/util/log.h"
#include "genie/util/stop_watch.h"
#include "genie/util/thread_manager.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "Spring";

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
  const std::string paired_end_str =
      preprocessor_.cp.paired_end ? "(paired-end)" : "(single-end)";
  UTILS_LOG(util::Logger::Severity::INFO,
            "Preprocessing done " + paired_end_str);
  std::vector<core::parameter::EncodingSet> params;
  auto loc_cp = preprocessor_.cp;
  util::Watch watch;
  core::stats::PerfStats stats = preprocessor_.stats;

  watch.Reset();
  UTILS_LOG(util::Logger::Severity::INFO, "Reordering");
  CallReorder(preprocessor_.temp_dir, loc_cp);
  stats.AddDouble("time-spring-reorder", watch.Check());

  watch.Reset();
  UTILS_LOG(util::Logger::Severity::INFO, "Encoding");
  CallEncoder(preprocessor_.temp_dir, loc_cp);
  stats.AddDouble("time-spring-encoding", watch.Check());

  watch.Reset();
  UTILS_LOG(util::Logger::Severity::INFO, "Generating read streams");
  GenerateReadStreams(preprocessor_.temp_dir, loc_cp, entropycoder_, params,
                      stats, write_out_streams_);
  stats.AddDouble("time-spring-gen-reads", watch.Check());

  if (preprocessor_.cp.preserve_quality || preprocessor_.cp.preserve_id) {
    watch.Reset();

    ReorderCompressQualityId(preprocessor_.temp_dir, loc_cp, qvcoder_,
                             namecoder_, entropycoder_, params, stats,
                             write_out_streams_);
    stats.AddDouble("time-spring-quality-name", watch.Check());
  }

  UTILS_LOG(util::Logger::Severity::INFO, "Writing encoded data to output");
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

  UTILS_LOG(util::Logger::Severity::INFO, "Finished!");
  FlushOut(pos);
}

// -----------------------------------------------------------------------------
Encoder::Encoder(const std::string& working_dir, const size_t num_thr,
                 const bool paired_end, const bool write_raw)
    : ReadEncoder(write_raw), preprocess_progress_printed_(0) {
  preprocessor_.Setup(working_dir, num_thr, paired_end);
  const std::string paired_end_str = paired_end ? "paired-end" : "single-end";
  UTILS_LOG(util::Logger::Severity::INFO,
            "Preprocessing (" + paired_end_str + ")");
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
