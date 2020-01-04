#include "encoding.h"

#include <util/log.h>

#include <coding/gabac-compressor.h>
#include <coding/local-assembly-encoder.h>
#include <format/mpegg_p2/mpegg-p-2-exporter.h>
#include <util/thread-manager.h>
#include <fstream>

namespace lae {

void encode(const ProgramOptions &programOptions) {
 /*   std::ifstream infile("test.sam");
    SamImporter samimporter(10000, &infile);
    LocalAssemblyEncoder laencoder(1000, false);
    GabacCompressor gabacCompressor;

    std::ofstream file("test.mpgb");
    MpeggP2Exporter exporter(&file);

    samimporter.setDrain(&laencoder);
    laencoder.setDrain(&gabacCompressor);
    gabacCompressor.setDrain(&exporter);

    ThreadManager threads(1, &samimporter);

    threads.run();*/
}

}  // namespace lae
