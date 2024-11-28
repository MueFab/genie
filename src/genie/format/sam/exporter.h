/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_EXPORTER_H_
#define SRC_GENIE_FORMAT_SAM_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------




#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "genie/core/format-exporter.h"
#include "genie/core/record/chunk.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/util/drain.h"
#include "genie/util/ordered-lock.h"
#include "genie/util/ordered-section.h"



// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::sam {

/**
 * @brief Module to export MPEG-G record to sam files
 */
class Exporter : public core::FormatExporter {
    util::OrderedLock lock;  //!< @brief Lock to ensure in order execution
    std::string fasta_file_path;
    std::string outputFilePath;
    std::optional<std::ofstream> output_stream;
    std::ostream *output_file = &std::cout;
    bool output_set = false;

 public:
    /**
     * @brief Unpaired mode
     * @param _file_1 Output file
     */
    explicit Exporter(std::string refFile, std::string output_file);

    /**
     * @brief Paired mode
     * @param _file_1 Output file #1
     * @param _file_2 Output file #2
     */
    Exporter();

    /**
     * @brief
     * @param id
     */
    void skipIn(const util::Section &id) override;

    /**
     * @brief Process one chunk of MPEGG records
     * @param records Input records
     * @param id Block identifier (for multithreading)
     */
    void flowIn(core::record::Chunk &&records, const util::Section &id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::sam

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_EXPORTER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
