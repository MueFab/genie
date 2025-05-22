/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/fastq/exporter.h"

#include <iostream>
#include <string>
#include <utility>

#include "genie/util/ordered_section.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::format::fastq {

// -----------------------------------------------------------------------------

Exporter::Exporter(std::ostream& file_1, bool compression) : file_{&file_1}, compress{compression} {}

// -----------------------------------------------------------------------------

Exporter::Exporter(std::ostream& file_1, std::ostream& file_2, bool compression)
    : file_{&file_1, &file_2}, compress{compression} {}

// -----------------------------------------------------------------------------

void Exporter::deflate_and_write(const char* s, std::streamsize n, std::ostream** file_ptr) {
  libdeflate_compressor * compressor = libdeflate_alloc_compressor(6);
    size_t max_compressed_size =
      libdeflate_gzip_compress_bound(compressor, n);
    void* buf = malloc(max_compressed_size);
    size_t written = libdeflate_gzip_compress(compressor, s, n, buf, max_compressed_size);
    (*file_ptr)->write(reinterpret_cast<const char*>(buf), static_cast<unsigned>(written));
    free(buf);
    libdeflate_free_compressor(compressor);
}


void Exporter::SkipIn(const util::Section& id) {
  [[maybe_unused]] util::OrderedSection sec(&lock_, id);
}

void Exporter::FlowIn(core::record::Chunk&& records, const util::Section& id) {
  core::record::Chunk data = std::move(records);
  GetStats().Add(data.GetStats());
  const util::Watch watch;

  size_t size_seq = 0;
  size_t size_qualities = 0;
  size_t size_name = 0;

  const auto num_files = file_.size();
  // ideally should be 2 for paired end, but we can handle 1 as well
  constexpr char read_name_suffix[2][3] = {"/1", "/2"};
  // suffix attached when paired end data but only one output fastq file
  bool second_read_flag = false;  // true when we are handling second read
  std::string file_1_buf;
  std::string file_2_buf;
  std::string * cur_file = &file_1_buf;

  for (const auto& i : data.GetData()) {
    second_read_flag = false;
    if (!i.IsRead1First()) {
      second_read_flag = true;
      if (num_files == 2) cur_file = &file_2_buf;
    }
    for (const auto& rec : i.GetSegments()) {
      // ID
      size_name += i.GetName().size();
      constexpr auto id_token = "@";
      *cur_file += id_token;
      *cur_file += i.GetName().c_str();
      if (i.GetNumberOfTemplateSegments() == 2 && num_files == 1) {
        *cur_file += read_name_suffix[second_read_flag];
      }
      *cur_file += "\n";

      // Sequence
      size_seq += rec.GetSequence().size();

      *cur_file += rec.GetSequence() + "\n";

      // Reserved Line
      constexpr auto reserved_token = "+";
      *cur_file += reserved_token;
      *cur_file += "\n" ;

      // Qualities
      if (!rec.GetQualities().empty()) {
        size_qualities += rec.GetQualities().front().size();
        *cur_file += rec.GetQualities().front();
      } else {
        // Make up default quality values
        size_qualities += rec.GetSequence().length();
        std::string qualities(rec.GetSequence().length(), '#');
          *cur_file += qualities;
      }
      *cur_file += "\n";

      second_read_flag = !second_read_flag;

      if (num_files == 2) {
        cur_file = (cur_file == &file_1_buf) ? &file_2_buf : &file_1_buf;
      }
    }
  }

  [[maybe_unused]] util::OrderedSection section(&lock_, id);
  if (compress) {
    deflate_and_write(file_1_buf.c_str(), static_cast<std::streamsize>(file_1_buf.size()), file_.data());
  } else {
    (*file_.data())->write(file_1_buf.c_str(), static_cast<std::streamsize>(file_1_buf.size()));
  }
  if (num_files == 2) {
    if (compress){
      deflate_and_write(file_2_buf.c_str(), static_cast<std::streamsize>(file_2_buf.size()), &file_.back());
    } else {
      (file_.back())->write(file_2_buf.c_str(), static_cast<std::streamsize>(file_2_buf.size()));
    }
  }

  GetStats().AddInteger("size-fastq-sequence", static_cast<int64_t>(size_seq));
  GetStats().AddInteger("size-fastq-name", static_cast<int64_t>(size_name));
  GetStats().AddInteger("size-fastq-quality",
                        static_cast<int64_t>(size_qualities));
  GetStats().AddInteger(
      "size-fastq-total",
      static_cast<int64_t>(size_qualities + size_name + size_seq));
  GetStats().AddDouble("time-fastq-export", watch.Check());
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::fastq

// -----------------------------------------------------------------------------
