/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_TRANSCODER_SAM_SAM_TO_MGREC_SORTER_H_
#define SRC_APPS_TRANSCODER_SAM_SAM_TO_MGREC_SORTER_H_

#include <genie/core/record/record.h>
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <fstream>
#include <string>

namespace genie {
namespace transcoder {
namespace sam {
namespace sam_to_mgrec {

class SubfileReader {
 private:
    uint64_t curr_mgrec_pos;
    std::ifstream reader;
    genie::util::BitReader bitreader;
    genie::core::record::Record rec;

 public:
    explicit SubfileReader(const std::string& fpath);

    ~SubfileReader();

    bool readRecord();

    genie::core::record::Record&& moveRecord();

    const genie::core::record::Record& getRecord() const;

    void writeRecord(genie::util::BitWriter& bitwriter);

    uint64_t getPos() const;

    bool good();
    void close();
};

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcoder
}  // namespace genie

#endif  // SRC_APPS_TRANSCODER_SAM_SAM_TO_MGREC_SORTER_H_
