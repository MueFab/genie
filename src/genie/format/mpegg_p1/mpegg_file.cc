#include "mpegg_file.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

MpeggFile::MpeggFile(std::vector<DatasetGroup>* x_datasetGroups) { datasetGroups = std::move(*x_datasetGroups); }

void MpeggFile::writeToFile(const std::string& outputFileName) {
    if (outputFileName.empty()) {
        fprintf(stdout, "empty outputFileName!!!\n");
    }
    fprintf(stdout, "Writing to file %s...\n", outputFileName.c_str());

    // TODO!!!

    fprintf(stdout, "Writing to file %s done!!!\n", outputFileName.c_str());
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie