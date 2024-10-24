/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_SAM_SAM_TO_MGREC_PROGRAM_OPTIONS_H_
#define SRC_GENIE_FORMAT_SAM_SAM_SAM_TO_MGREC_PROGRAM_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <string>
#include <thread>

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp::transcode_sam::sam::sam_to_mgrec {

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

    Config()
        : verbosity_level(0),
          tmp_dir_path("/tmp"),
          forceOverwrite(false),
          help(false),
          no_ref(false),
          clean(false),
          num_threads(std::thread::hardware_concurrency()) {}

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
    bool forceOverwrite;          //!< @brief
    bool help;                    //!< @brief
    bool no_ref;                  //!< @brief
    bool clean;                   //!< @brief
    uint32_t num_threads;         //!< @brief

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

}  // namespace genieapp::transcode_sam::sam::sam_to_mgrec

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_SAM_SAM_TO_MGREC_PROGRAM_OPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
