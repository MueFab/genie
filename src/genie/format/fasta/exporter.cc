/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/fasta/exporter.h"

#include <vector>

#include "genie/format/fasta/fasta_source.h"
#include "genie/util/thread_manager.h"

// -----------------------------------------------------------------------------

namespace genie::format::fasta {

// -----------------------------------------------------------------------------

Exporter::Exporter(core::ReferenceManager* ref_mgr, std::ostream* out,
                   const size_t num_threads)
    : ref_mgr_(ref_mgr), outfile_(out), num_threads_(num_threads) {}

// -----------------------------------------------------------------------------

void Exporter::FlushIn(uint64_t&) {
  util::ThreadManager mgr(num_threads_);
  FastaSource source(outfile_, ref_mgr_);
  std::vector<util::OriginalSource*> vec;
  vec.push_back(&source);
  mgr.SetSource(vec);
  mgr.Run();
}

// -----------------------------------------------------------------------------

void Exporter::FlowIn(core::record::Chunk&&, const util::Section&) {}

// -----------------------------------------------------------------------------

}  // namespace genie::format::fasta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
