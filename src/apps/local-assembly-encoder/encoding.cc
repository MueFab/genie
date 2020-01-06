#include "encoding.h"

#include <util/log.h>

#include <coding/gabac-compressor.h>
#include <coding/gabac-decompressor.h>
#include <coding/local-assembly-decoder.h>
#include <coding/local-assembly-encoder.h>
#include <format/mpegg_p2/mpegg-p-2-exporter.h>
#include <format/mpegg_p2/mpegg-p-2-importer.h>
#include <format/sam/sam-exporter.h>
#include <format/sam/sam-importer.h>
#include <util/thread-manager.h>
#include <fstream>

namespace lae {

void encode(const ProgramOptions &programOptions) {
    if (!programOptions.decompression) {
        std::ifstream infile(programOptions.inputFilePath);
        const size_t RECORDS_PER_BLOCK = 10000;
        format::sam::SamImporter importer(RECORDS_PER_BLOCK, infile);

        const size_t LOCAL_ASSEMBLY_BUFFER_SIZE = 2000;
        LocalAssemblyEncoder encoder(LOCAL_ASSEMBLY_BUFFER_SIZE, false);

        GabacCompressor compressor;

        std::ofstream outfile(programOptions.outputFilePath);
        MpeggP2Exporter exporter(&outfile);

        importer.setDrain(&encoder);
        encoder.setDrain(&compressor);
        compressor.setDrain(&exporter);

        const size_t NUM_THREADS = 8;
        ThreadManager threads(NUM_THREADS, &importer);

        threads.run();
    } else {
        std::ifstream infile(programOptions.inputFilePath);
        MpeggP2Importer importer(infile);

        LocalAssemblyDecoder decoder;

        GabacDecompressor decompressor;

        std::ofstream outfile(programOptions.outputFilePath);
        SamExporter exporter(format::sam::SamFileHeader::createDefaultHeader(), outfile);

        importer.setDrain(&decompressor);
        decompressor.setDrain(&decoder);
        decoder.setDrain(&exporter);

        ThreadManager threads(programOptions.num_threads, &importer);

        threads.run();
    }

    std::cout << "Finished successfully!" << std::endl;
}

}  // namespace lae
