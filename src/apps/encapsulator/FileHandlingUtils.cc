/**
 * @file
 * @copyright This file is part of Genie. See LICENSE for more details.
 */

#include "FileHandlingUtils.h"
#include <genie/format/mgb/importer.h>
#include "genie/core/parameter/parameter_set.h"
#include "genie/read/lowlatency/decoder.h"
#include "genie/format//mpegg_p1/dataset_group.h"
#include "genie/format/mgb/data-unit-factory.h"
#include "genie/format/mpegg_p1/dataset.h"
#include "genie/format/mpegg_p1/mpegg_file.h"

int createMPEGGFileNoMITFromByteStream(const std::string& fileName, const std::string& outputFileName) {
    fprintf(stdout, "Bytestream file: %s\n", fileName.c_str());

    std::ifstream inputFilestream;
    inputFilestream.open(fileName, std::ios::binary);
    if (!inputFilestream.good()) {
        fprintf(stderr, "File could not be opened!\n");
        return -1;
    }

    genie::util::BitReader inputFileBitReader(inputFilestream);

    std::vector<genie::format::mgb::AccessUnit> accessUnits;

    genie::core::ReferenceManager refmgr(16);
    genie::read::lowlatency::Decoder decoder;
    genie::format::mgb::Importer importer(inputFilestream, &refmgr, &decoder, false );

    auto dataUnitFactory = genie::format::mgb::DataUnitFactory(&refmgr, &importer, false);
    while (true) {
        auto au = dataUnitFactory.read(inputFileBitReader);
        if (!au) {
            break;
        }
        accessUnits.push_back(std::move(au.get()));
    }

    for (auto const& au : accessUnits) {
        fprintf(stdout, "\ndata_unit_type:%u\n", (uint8_t)au.getDataUnitType());
    }

    // ---------------------------------------------------------------------------------------------------------------------

    // Dataset consists of:
    // - DatasetHeader
    // - DatasetParameterSet
    // - AccessUnit[]

    std::vector<genie::format::mpegg_p1::Dataset> datasets;
    datasets.emplace_back(dataUnitFactory, accessUnits, 22);
    // std::cout << datasets.front().getDatasetParameterSetDatasetID() << std::endl;

    // DatasetGroup consists of:
    // - DatasetGroupHeader
    // - Dataset[]

    std::vector<genie::format::mpegg_p1::DatasetGroup> datasetGroups;
    datasetGroups.emplace_back(&datasets, 1);

    // test output
    fprintf(stdout, "%u\n", datasetGroups.front().getDatasets().front().getDatasetHeader().getDatasetId());
    fprintf(stdout, "%u\n", datasetGroups.front().getDatasets().front().getDatasetHeader().getDatasetGroupId());
    fprintf(stdout, "%u\n", datasetGroups.front().getDatasets().front().getDatasetParameterSetDatasetID());
    fprintf(stdout, "%u\n", datasetGroups.front().getDatasets().front().getDatasetParameterSetDatasetGroupID());

    // MpeggFile consists of:
    // - FileHeader
    // - DatasetGroup[]

    genie::format::mpegg_p1::MpeggFile mpeggFile(&datasetGroups);

    // test output
    fprintf(stdout, "%u\n",
            mpeggFile.getDatasetGroups().front().getDatasets().front().getDatasetHeader().getDatasetGroupId());

    // FIXME: try to open file at function start to avoid decompressing without writing?!?
    std::string outputFileNameNew(outputFileName);
    if (outputFileNameNew.empty()) {
        fprintf(stdout, "empty outputFileName!!!\n");
        outputFileNameNew = "";  // FIXME: add default path/filename and remove else clause?!?
    } else {
        fprintf(stdout, "Writing to file %s...\n", outputFileNameNew.c_str());

        std::filebuf fb;
        fb.open(outputFileNameNew, std::ios::out);
        if (!fb.is_open()) {
            fprintf(stdout, "File is not open!!!\n");
        }
        std::ostream os(&fb);
        genie::util::BitWriter outputFileBitReader(&os);

        mpeggFile.writeToFile(outputFileBitReader);

        fb.close();
        fprintf(stdout, "Writing to file %s done!!!\n", outputFileNameNew.c_str());
    }

    return 0;
}
