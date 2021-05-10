#ifndef TRANSCODER_PROGRAM_OPTIONS_H_
#define TRANSCODER_PROGRAM_OPTIONS_H_

#include <string>

namespace transcoder {

class ProgramOptions {
   public:
    enum format: std::uint8_t {
        sam = 0,
    };

    ProgramOptions(int argc, char *argv[]);

    ~ProgramOptions();

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

}  // namespace transcoder

#endif  // TRANSCODER_PROGRAM_OPTIONS_H_
