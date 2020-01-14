#include "encoding.h"

#include <format/mpegg_rec/mgrecs-importer.h>
#include <format/sam/sam-exporter.h>
#include <format/sam/sam-header.h>
#include <util/thread-manager.h>
#include <fstream>
#include <string>

namespace ureads_encoder {

void encode(const ProgramOptions& programOptions) {
    (void)programOptions;
#if 1
    {
        std::ifstream input("test_out.mgrec");
        std::ofstream output("test_out.sam");

        MgrecsImporter importer(1000, input);
        SamExporter exporter(format::sam::SamFileHeader::createDefaultHeader(), output);
        importer.setDrain(&exporter);
        ThreadManager manager(1, &importer);
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
        format::sam::SamImporter importer(1000, input);
        LocalAssemblyEncoder encoder(1000, false);
        GabacCompressor compressor;
        MpeggP2Exporter exporter(&output);

        importer.setDrain(&encoder);
        encoder.setDrain(&compressor);
        compressor.setDrain(&exporter);
        ThreadManager manager(1, &importer);
        manager.run();
    }
    {
        std::ifstream input("test_out.mgb");
        std::ofstream output("test_out.sam");
        MpeggP2Importer importer(input);
        LocalAssemblyDecoder decoder;
        GabacDecompressor decompressor;
        SamExporter exporter(format::sam::SamFileHeader::createDefaultHeader(), output);
        importer.setDrain(&decompressor);
        decompressor.setDrain(&decoder);
        decoder.setDrain(&exporter);

        ThreadManager manager(1, &importer);
        manager.run();
    }
#endif
}

}  // namespace ureads_encoder
