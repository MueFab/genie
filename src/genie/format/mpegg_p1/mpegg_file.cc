#include "mpegg_file.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

MpeggFile::MpeggFile(std::vector<DatasetGroup>* x_datasetGroups) : fileHeader() {
    datasetGroups = std::move(*x_datasetGroups);
}

MpeggFile::MpeggFile(std::vector<DatasetGroup>* x_datasetGroups, std::vector<std::string>* compatible_brand)
    : fileHeader(compatible_brand) {
    datasetGroups = std::move(*x_datasetGroups);
}

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