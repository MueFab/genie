#include "encoding.h"

#include <genie/entropy/gabac/gabac-compressor.h>
#include <genie/entropy/gabac/gabac-decompressor.h>
#include <genie/format/mgb/exporter.h>
#include <genie/format/mgb/importer.h>
#include <genie/format/sam/exporter.h>
#include <genie/format/sam/importer.h>
#include <genie/module/manager.h>
#include <genie/read/localassembly/decoder.h>
#include <genie/read/localassembly/encoder.h>
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <genie/util/exceptions.h>
#include <genie/util/thread-manager.h>
#include <fstream>
#include <sstream>
#include <string>

namespace ureads_encoder {

void encode(const ProgramOptions& programOptions) {
    (void)programOptions;
#if 0
    {
        std::ifstream input("test_out.mgrec");
        std::ofstream output("test_out.sam");

        genie::record::MgrecsImporter importer(1000, input);
        genie::sam::SamExporter exporter(genie::sam::SamFileHeader::createDefaultHeader(), output);
        importer.setDrain(&exporter);
        util::ThreadManager manager(1, &importer);
        manager.run();
    }
#else
    genie::module::detect();
    {
        std::stringstream buffer;
        genie::util::BitWriter writer(&buffer);
        genie::util::BitReader reader(buffer);
        for (size_t bits = 1; bits <= 64; ++bits) {
            for (size_t number = 0; number < (1u << bits) && number < 100; ++number) {
                writer.write(number, bits);
            }
            writer.flush();
            for (size_t number = 0; number < (1u << bits) && number < 100; ++number) {
                UTILS_DIE_IF(reader.read(bits) != number,
                             "Fail at bits " + std::to_string(bits) + " number " + std::to_string(number));
            }
            reader.flush();
        }
    }
    {
        std::ifstream input("test.sam");
        std::ofstream output("test_out.mgb");
        genie::format::sam::Importer importer(1000, input);
        genie::read::localassembly::Encoder encoder(1000, false);
        genie::entropy::gabac::GabacCompressor compressor;
        genie::format::mgb::Exporter exporter(&output);

        importer.setDrain(&encoder);
        encoder.setDrain(&compressor);
        compressor.setDrain(&exporter);
        genie::util::ThreadManager manager(1, &importer);
        manager.run();
    }
    {
        std::ifstream input("test_out.mgb");
        std::ofstream output("test_out.sam");
        genie::format::mgb::Importer importer(input);
        genie::read::localassembly::Decoder decoder;
        genie::entropy::gabac::GabacDecompressor decompressor;
        genie::format::sam::Exporter exporter(genie::format::sam::header::Header::createDefaultHeader(), output);
        importer.setDrain(&decompressor);
        decompressor.setDrain(&decoder);
        decoder.setDrain(&exporter);

        genie::util::ThreadManager manager(1, &importer);
        manager.run();
    }
#endif
}

}  // namespace ureads_encoder
