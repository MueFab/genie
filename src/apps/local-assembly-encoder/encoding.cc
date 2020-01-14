#include "encoding.h"

#include <util/log.h>

#include <gabac/gabac-compressor.h>
#include <gabac/gabac-decompressor.h>
#include <local_assembly/local-assembly-decoder.h>
#include <local_assembly/local-assembly-encoder.h>
#include <mpegg_p2/mpegg-p-2-exporter.h>
#include <mpegg_p2/mpegg-p-2-importer.h>
#include <sam/sam-exporter.h>
#include <sam/sam-importer.h>
#include <util/thread-manager.h>
#include <fstream>

namespace lae {

void encode(const ProgramOptions &programOptions) {
    if (!programOptions.decompression) {
        std::ifstream infile(programOptions.inputFilePath);
        const size_t RECORDS_PER_BLOCK = 10000;
        genie::sam::SamImporter importer(RECORDS_PER_BLOCK, infile);

        const size_t LOCAL_ASSEMBLY_BUFFER_SIZE = 2000;
        genie::local_assembly::LocalAssemblyEncoder encoder(LOCAL_ASSEMBLY_BUFFER_SIZE, false);

        genie::gabac::GabacCompressor compressor;

        std::ofstream outfile(programOptions.outputFilePath);
        genie::mpegg_p2::MpeggP2Exporter exporter(&outfile);

        importer.setDrain(&encoder);
        encoder.setDrain(&compressor);
        compressor.setDrain(&exporter);

        util::ThreadManager threads(programOptions.num_threads, &importer);

        threads.run();
    } else {
        std::ifstream infile(programOptions.inputFilePath);
        genie::MpeggP2Importer importer(infile);

        genie::local_assembly::LocalAssemblyDecoder decoder;

        genie::gabac::GabacDecompressor decompressor;

        std::ofstream outfile(programOptions.outputFilePath);
        genie::sam::SamExporter exporter(genie::sam::SamFileHeader::createDefaultHeader(), outfile);

        importer.setDrain(&decompressor);
        decompressor.setDrain(&decoder);
        decoder.setDrain(&exporter);

        util::ThreadManager threads(programOptions.num_threads, &importer);

        threads.run();
    }

    std::cout << "Finished successfully!" << std::endl;
}

}  // namespace lae
