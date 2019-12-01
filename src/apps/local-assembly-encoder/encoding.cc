#include "encoding.h"

#include <util/log.h>
#include <format/sam/sam-record.h>

#include <fstream>
#include <format/sam/sam-importer.h>
#include <coding/local-assembly-encoder.h>
#include <coding/gabac-compressor.h>
#include <format/mpegg_p2/mpegg-p-2-exporter.h>

namespace lae {

 /*   void pack(const ProgramOptions &programOptions, std::unique_ptr<StreamContainer> container, uint32_t read_length,
              uint32_t readNum, bool paired) {
        using namespace format::mpegg_p2;

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // CREATE Part 2 units
        /////////////////////////////////////////////////////////////////////////////////////////////////////


        std::ofstream ofstr(programOptions.outputFilePath);
        util::BitWriter bw(&ofstr);

        const uint8_t PARAMETER_SET_ID = 0;
        const uint32_t READ_LENGTH = read_length;
        const bool QV_PRESENT = false;

        ParameterSet ps = createQuickParameterSet(PARAMETER_SET_ID, READ_LENGTH, paired, QV_PRESENT, programOptions.type, configs, false);
        auto crps = util::make_unique<ParameterSetCrps>(ParameterSetCrps::CrAlgId::LOCAL_ASSEMBLY);
        crps->setCrpsInfo(util::make_unique<CrpsInfo>(0, 1000));
        ps.setCrps(std::move(crps));
        ps.write(&bw);

        const uint32_t ACCESS_UNIT_ID = 0;
        AccessUnit au = createQuickAccessUnit(ACCESS_UNIT_ID, PARAMETER_SET_ID, readNum, programOptions.type, DataUnit::DatasetType::ALIGNED, &generated_streams);
        au.write(&bw);
    } */

    void encode(const ProgramOptions &programOptions) {

        std::ifstream infile("test.sam");
        SamImporter samimporter(10000, &infile);
        LocalAssemblyEncoder laencoder(1000, false);
        GabacCompressor gabacCompressor;

        std::ofstream file("test.mpgb");
        MpeggP2Exporter exporter(&file);

        samimporter.setDrain(&laencoder);
        laencoder.setDrain(&gabacCompressor);
        gabacCompressor.setDrain(&exporter);

        while(samimporter.pump()) {
            std::cout << "Running..." << std::endl;
        }

    }

}  // namespace lae
