#include <genie/entropy/gabac/gabac-compressor.h>
#include <genie/entropy/gabac/gabac-decompressor.h>
#include <genie/format/mgb/exporter.h>
#include <genie/format/mgb/importer.h>
#include <genie/format/sam/exporter.h>
#include <genie/format/sam/importer.h>
#include <genie/module/manager.h>
#include <genie/name/tokenizer/encoder.h>
#include <genie/quality/qvwriteout/encoder.h>
#include <genie/read/localassembly/decoder.h>
#include <genie/read/localassembly/encoder.h>
#include <genie/util/thread-manager.h>
#include <fstream>
#include "program-options.h"
namespace lae {

void encode(const ProgramOptions &programOptions) {
    genie::module::detect();
    if (!programOptions.decompression) {
        std::ifstream infile(programOptions.inputFilePath);
        const size_t RECORDS_PER_BLOCK = 10000;
        genie::format::sam::Importer importer(RECORDS_PER_BLOCK, infile);

        const size_t LOCAL_ASSEMBLY_BUFFER_SIZE = 2000;
        genie::quality::qvwriteout::Encoder qvencoder;
        genie::name::tokenizer::Encoder namecoder;
        genie::read::localassembly::Encoder encoder(LOCAL_ASSEMBLY_BUFFER_SIZE, false, &qvencoder, &namecoder);

        genie::entropy::gabac::GabacCompressor compressor;

        std::ofstream outfile(programOptions.outputFilePath);
        genie::format::mgb::Exporter exporter(&outfile);

        importer.setDrain(&encoder);
        encoder.setDrain(&compressor);
        compressor.setDrain(&exporter);

        genie::util::ThreadManager threads(programOptions.num_threads, &importer);

        threads.run();
    } else {
        std::ifstream infile(programOptions.inputFilePath);
        genie::format::mgb::Importer importer(infile);

        genie::read::localassembly::Decoder decoder;

        genie::entropy::gabac::GabacDecompressor decompressor;

        std::ofstream outfile(programOptions.outputFilePath);
        genie::format::sam::Exporter exporter(genie::format::sam::header::Header(), outfile);

        importer.setDrain(&decompressor);
        decompressor.setDrain(&decoder);
        decoder.setDrain(&exporter);

        genie::util::ThreadManager threads(programOptions.num_threads, &importer);

        threads.run();
    }

    std::cout << "Finished successfully!" << std::endl;
}

}  // namespace lae
