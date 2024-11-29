/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#define NOMINMAX  // NOLINT
#include "apps/genie/capsulator/main.h"

#include <filesystem>  // NOLINT
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "apps/genie/capsulator/program_options.h"
#include "format/mgb/mgb_file.h"
#include "format/mgg/encapsulator/decapsulated_file.h"
#include "format/mgg/encapsulator/encapsulated_file.h"
#include "genie/format/mgb/raw_reference.h"
#include "genie/format/mgg/mgg_file.h"
#include "genie/util/runtime_exception.h"
#include "util/string_helpers.h"

// -----------------------------------------------------------------------------

namespace genie_app::capsulator {

// -----------------------------------------------------------------------------
void encapsulate(ProgramOptions& options) {
  auto version = genie::core::MpegMinorVersion::kV2000;

  auto inputs = genie::util::Tokenize(options.input_file_, ';');
  auto input_file =
      genie::format::mgg::encapsulator::EncapsulatedFile(inputs, version);
  auto mgg_file = input_file.assemble(version);

  std::ofstream output_stream(options.output_file_);
  genie::util::BitWriter writer(output_stream);

  mgg_file.print_debug(std::cerr, 100);

  mgg_file.Write(writer);
}

// -----------------------------------------------------------------------------
void decapsulate(ProgramOptions& options) {
  genie::format::mgg::encapsulator::DecapsulatedFile ret(options.input_file_);
  std::string global_output_prefix =
      options.output_file_.substr(0, options.output_file_.find_last_of('.'));

  for (auto& grp : ret.GetGroups()) {
    for (auto& [fst, snd] : grp.GetData()) {
      std::string local_output_prefix = global_output_prefix + "." +
                                        std::to_string(grp.GetId()) + "." +
                                        std::to_string(fst);
      auto meta_json = snd.second.ToJson().dump(4);
      std::ofstream json_file(local_output_prefix + ".mgb.json");
      json_file.write(meta_json.data(),
                      static_cast<std::streamsize>(meta_json.length()));

      std::ofstream mgb_output_file(local_output_prefix + ".mgb");
      genie::util::BitWriter mgb_output_writer(mgb_output_file);
      snd.first.Write(mgb_output_writer);
    }
  }
}

// -----------------------------------------------------------------------------
int main(const int argc, char* argv[]) {
  ProgramOptions p_opts(argc, argv);

  try {
    if (p_opts.input_file_.substr(p_opts.input_file_.length() - 3) == "mgb" &&
        p_opts.output_file_.substr(p_opts.output_file_.length() - 3) == "mgg") {
      encapsulate(p_opts);
    } else if (p_opts.input_file_.substr(p_opts.input_file_.length() - 3) ==
                   "mgg" &&
               p_opts.output_file_.substr(p_opts.output_file_.length() - 3) ==
                   "mgb") {
      decapsulate(p_opts);
    }
  } catch (genie::util::RuntimeException& e) {
    std::cerr << e.Msg() << std::endl;
    return -1;
  } catch (std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    return -1;
  } catch (...) {
    std::cerr << "Unknown error" << std::endl;
    return -1;
  }
  return 0;
}

// -----------------------------------------------------------------------------

}  // namespace genie_app::capsulator

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
