/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_GENIE_TRANSCODE_SAM_SAM_SAM_TO_MGREC_PROGRAM_OPTIONS_H_
#define SRC_APPS_GENIE_TRANSCODE_SAM_SAM_SAM_TO_MGREC_PROGRAM_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace transcode_sam {
namespace sam {
namespace sam_to_mgrec {

/**
 * @brief
 */
class Config {
 public:
    /**
     * @brief
     */
    enum format : std::uint8_t {
        sam = 0,
    };

    /**
     * @brief
     * @param argc
     * @param argv
     */
    Config(int argc, char *argv[]);

    /**
     * @brief
     */
    ~Config();

 public:
    int verbosity_level;          //!< @brief
    std::string tmp_dir_path;     //!< @brief
    std::string fasta_file_path;  //!< @brief
    std::string inputFile;        //!< @brief
    std::string outputFile;       //!< @brief
    bool forceOverwrite;

 private:
    /**
     * @brief
     * @param argc
     * @param argv
     */
    void processCommandLine(int argc, char *argv[]);

    /**
     * @brief
     */
    void validate();
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcode_sam
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_TRANSCODE_SAM_SAM_SAM_TO_MGREC_PROGRAM_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
