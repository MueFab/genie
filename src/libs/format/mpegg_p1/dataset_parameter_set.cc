#include "dataset_parameter_set.h"

namespace format {
namespace mpegg_p1 {
DatasetParameterSet::DatasetParameterSet(format::mpegg_p2::ParameterSet& parameterSet)
    : format::mpegg_p2::ParameterSet(parameterSet) {}

}  // namespace mpegg_p1
}  // namespace format