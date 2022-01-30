/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/format/mpegg_p1/dataset/class_description.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

#include "dataset/dataset.h"
#include "genie/format/mpegg_p1/dataset_group_header.h"
#include "label_list.h"
#include "reference.h"
#include "reference_metadata.h"
#include "boost/optional/optional.hpp"
#include "dataset_group_metadata.h"
#include "dataset_group_protection.h"
#include "dataset.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DatasetGroup {
 private:
  DatasetGroupHeader header;
  std::vector<Reference> references;
  std::vector<ReferenceMetadata> reference_metadatas;
  boost::optional<LabelList> labels;
  boost::optional<DatasetGroupMetadata> metadata;
  boost::optional<DatasetGroupProtection> protection;
  std::vector<Dataset> dataset;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
