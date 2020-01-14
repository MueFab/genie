#include "encoding.h"

#include <gabac/gabac-compressor.h>
#include <gabac/gabac-decompressor.h>
#include <local_assembly/local-assembly-decoder.h>
#include <local_assembly/local-assembly-encoder.h>
#include <mpegg_p2/mpegg-p-2-exporter.h>
#include <mpegg_p2/mpegg-p-2-importer.h>
#include <mpegg_rec/mgrecs-importer.h>
#include <sam/sam-exporter.h>
#include <sam/sam-header.h>
#include <sam/sam-importer.h>
#include <util/thread-manager.h>
#include <fstream>
#include <string>

namespace ureads_encoder {

void encode(const ProgramOptions& programOptions) {
    (void)programOptions;
#if 0
    {
        std::ifstream input("test_out.mgrec");
        std::ofstream output("test_out.sam");

        genie::mpegg_rec::MgrecsImporter importer(1000, input);
        genie::sam::SamExporter exporter(genie::sam::SamFileHeader::createDefaultHeader(), output);
        importer.setDrain(&exporter);
        util::ThreadManager manager(1, &importer);
        manager.run();
    }
#else
    {
        std::stringstream buffer;
        util::BitWriter writer(&buffer);
        util::BitReader reader(buffer);
        for (size_t bits = 1; bits <= 64; ++bits) {
            for (size_t number = 0; number < (1u << bits) && number < 100; ++number) {
                writer.write(number, bits);
            }
            writer.flush();
            for (size_t number = 0; number < (1u << bits) && number < 100; ++number) {
                if (reader.read(bits) != number) {
                    UTILS_DIE("Fail at bits " + std::to_string(bits) + " number " + std::to_string(number));
                }
            }
            reader.flush();
        }
    }
    {
        std::ifstream input("test.sam");
        std::ofstream output("test_out.mgb");
        genie::sam::SamImporter importer(1000, input);
        genie::local_assembly::LocalAssemblyEncoder encoder(1000, false);
        genie::gabac::GabacCompressor compressor;
        genie::mpegg_p2::MpeggP2Exporter exporter(&output);

        importer.setDrain(&encoder);
        encoder.setDrain(&compressor);
        compressor.setDrain(&exporter);
        util::ThreadManager manager(1, &importer);
        manager.run();
    }
    {
        std::ifstream input("test_out.mgb");
        std::ofstream output("test_out.sam");
        genie::MpeggP2Importer importer(input);
        genie::local_assembly::LocalAssemblyDecoder decoder;
        genie::gabac::GabacDecompressor decompressor;
        genie::sam::SamExporter exporter(genie::sam::SamFileHeader::createDefaultHeader(), output);
        importer.setDrain(&decompressor);
        decompressor.setDrain(&decoder);
        decoder.setDrain(&exporter);

        util::ThreadManager manager(1, &importer);
        manager.run();
    }
#endif
}

}  // namespace ureads_encoder
