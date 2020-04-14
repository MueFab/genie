#include "encoding.h"

#include <genie/entropy/gabac/gabac-compressor.h>
#include <genie/entropy/gabac/gabac-decompressor.h>
#include <genie/format/fastq/exporter.h>
#include <genie/format/fastq/importer.h>
#include <genie/format/mgb/exporter.h>
#include <genie/format/mgb/importer.h>
#include <genie/module/manager.h>
#include <genie/name/tokenizer/encoder.h>
#include <genie/quality/qvwriteout/encoder.h>
#include <genie/read/lowlatency/decoder.h>
#include <genie/read/lowlatency/encoder.h>
#include <genie/read/spring/spring-encoder.h>
#include <genie/util/thread-manager.h>
#include <genie/core/stats/perf-stats.h>
#include <fstream>
#include <string>

namespace lowlatency_encoder {

void encode(const ProgramOptions& programOptions) {
    genie::module::detect();
    if (!programOptions.decompression) {
        std::ifstream infile(programOptions.inputFilePath);
        UTILS_DIE_IF(!infile, "Could not open input file");
        const size_t RECORDS_PER_BLOCK = 10000;

        genie::quality::qvwriteout::Encoder qvencoder;
        genie::name::tokenizer::Encoder nameencoder;

        genie::core::stats::FastqStats stats(false);
        genie::read::spring::SpringEncoder encoder(&qvencoder, &nameencoder, "./", &stats);

        genie::entropy::gabac::GabacCompressor compressor;

        std::ofstream outfile(programOptions.outputFilePath);
        std::ifstream pairedfile(programOptions.pairedPath);
        std::unique_ptr<genie::format::fastq::Importer> importer;
        if (programOptions.pairedPath.empty()) {
            importer = genie::util::make_unique<genie::format::fastq::Importer>(RECORDS_PER_BLOCK, infile);
        } else {
            UTILS_DIE_IF(!pairedfile, "Could not open paired input file");
            importer = genie::util::make_unique<genie::format::fastq::Importer>(RECORDS_PER_BLOCK, infile, pairedfile);
        }
        genie::format::mgb::Exporter exporter(&outfile);

        importer->setDrain(&encoder);
        encoder.setDrain(&compressor);
        compressor.setDrain(&exporter);

        genie::util::ThreadManager threads(programOptions.num_threads, importer.get());

        threads.run();
    } else {
        std::ifstream infile(programOptions.inputFilePath);
        genie::format::mgb::Importer importer(infile);

        genie::read::lowlatency::Decoder decoder;

        genie::entropy::gabac::GabacDecompressor decompressor;

        std::ofstream outfile(programOptions.outputFilePath);
        std::ofstream pairedfile(programOptions.pairedPath);
        std::unique_ptr<genie::format::fastq::Exporter> exporter;
        if (programOptions.pairedPath.empty()) {
            exporter = genie::util::make_unique<genie::format::fastq::Exporter>(outfile);
        } else {
            exporter = genie::util::make_unique<genie::format::fastq::Exporter>(outfile, pairedfile);
        }

        importer.setDrain(&decompressor);
        decompressor.setDrain(&decoder);
        decoder.setDrain(exporter.get());

        genie::util::ThreadManager threads(programOptions.num_threads, &importer);

        threads.run();
    }
}

}  // namespace lowlatency_encoder
