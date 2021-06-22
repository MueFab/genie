/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_TRANSCODER_SAM_SAM_TO_MGREC_PROGRAM_OPTIONS_H_
#define SRC_APPS_TRANSCODER_SAM_SAM_TO_MGREC_PROGRAM_OPTIONS_H_

#include <string>

namespace genie {
namespace transcoder {
namespace sam {
namespace sam_to_mgrec {

class Config {
 public:
    enum format : std::uint8_t {
        sam = 0,
    };

    Config(int argc, char *argv[]);

    ~Config();

 public:
    int verbosity_level;
    std::string tmp_dir_path;
    std::string fasta_file_path;
    std::string sam_file_path;
    std::string mgrec_file_path;

 private:
    void processCommandLine(int argc, char *argv[]);

    void validate() const;
};

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcoder
}  // namespace genie

#endif  // SRC_APPS_TRANSCODER_SAM_SAM_TO_MGREC_PROGRAM_OPTIONS_H_
