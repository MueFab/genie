/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/fastq/exporter.h"

#include <string>
#include <utility>

#include "genie/util/ordered_section.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::format::fastq {

// -----------------------------------------------------------------------------
Exporter::Exporter(std::ostream& file_1) : file_{&file_1} {}

// -----------------------------------------------------------------------------
Exporter::Exporter(std::ostream& file_1, std::ostream& file_2)
    : file_{&file_1, &file_2} {}

// -----------------------------------------------------------------------------
void Exporter::SkipIn(const util::Section& id) {
  [[maybe_unused]] util::OrderedSection sec(&lock_, id);
}

void Exporter::FlowIn(core::record::Chunk&& records, const util::Section& id) {
  core::record::Chunk data = std::move(records);
  GetStats().Add(data.GetStats());
  const util::Watch watch;
  [[maybe_unused]] util::OrderedSection section(&lock_, id);
  size_t size_seq = 0;
  size_t size_qualities = 0;
  size_t size_name = 0;

  const auto num_files = file_.size();
  // ideally should be 2 for paired end, but we can handle 1 as well
  constexpr char read_name_suffix[2][3] = {"/1", "/2"};
  // suffix attached when paired end data but only one output fastq file
  bool second_read_flag = false;  // true when we are handling second read

  for (const auto& i : data.GetData()) {
    auto file_ptr = file_.data();
    second_read_flag = false;
    if (!i.IsRead1First()) {
      second_read_flag = true;
      if (num_files == 2) file_ptr = &file_.back();
    }
    for (const auto& rec : i.GetSegments()) {
      // ID
      size_name += i.GetName().size();
      constexpr auto id_token = "@";
      (*file_ptr)->write(id_token, 1);
      (*file_ptr)->write(i.GetName().c_str(),
                         static_cast<std::streamsize>(i.GetName().length()));
      if (i.GetNumberOfTemplateSegments() == 2 && num_files == 1)
        (*file_ptr)->write(read_name_suffix[second_read_flag], 2);
      (*file_ptr)->write("\n", 1);

      // Sequence
      size_seq += rec.GetSequence().size();
      (*file_ptr)->write(
          rec.GetSequence().c_str(),
          static_cast<std::streamsize>(rec.GetSequence().length()));
      (*file_ptr)->write("\n", 1);

      // Reserved Line
      constexpr auto reserved_token = "+";
      (*file_ptr)->write(reserved_token, 1);
      (*file_ptr)->write("\n", 1);

      // Qualities
      if (!rec.GetQualities().empty()) {
        size_qualities += rec.GetQualities().front().size();
        (*file_ptr)->write(
            rec.GetQualities().front().c_str(),
            static_cast<std::streamsize>(rec.GetQualities().front().length()));
      } else {
        // Make up default quality values
        size_qualities += rec.GetSequence().length();
        std::string qualities(rec.GetSequence().length(), '#');
        (*file_ptr)->write(qualities.c_str(),
                           static_cast<std::streamsize>(qualities.length()));
      }
      (*file_ptr)->write("\n", 1);
      second_read_flag = !second_read_flag;
      if (num_files == 2) {
        if (i.IsRead1First()) {
          file_ptr++;
        } else {
          file_ptr--;
        }
      }
    }
  }

  GetStats().AddInteger("Size-fastq-sequence", static_cast<int64_t>(size_seq));
  GetStats().AddInteger("Size-fastq-name", static_cast<int64_t>(size_name));
  GetStats().AddInteger("Size-fastq-quality",
                        static_cast<int64_t>(size_qualities));
  GetStats().AddInteger(
      "Size-fastq-total",
      static_cast<int64_t>(size_qualities + size_name + size_seq));
  GetStats().AddDouble("time-fastq-export", watch.Check());
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::fastq

// -----------------------------------------------------------------------------
