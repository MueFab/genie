#ifndef TRANSCODER_PROGRAM_OPTIONS_H_
#define TRANSCODER_PROGRAM_OPTIONS_H_

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

}
}
}
}

#endif  // TRANSCODER_PROGRAM_OPTIONS_H_
