/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GENOTYPE_TEST_HELPERS_H
#define GENIE_GENOTYPE_TEST_HELPERS_H

#include "genie/backend/test_utils.h"

namespace genie::genotype {

using genie::backend::get_mat_shape;
using genie::backend::get_mat_element;
using genie::backend::set_mat_element;

using genie::backend::test_utils::PrintTo;
using genie::backend::test_utils::equal;
using genie::backend::test_utils::sum;
using genie::backend::test_utils::amax;
using genie::backend::test_utils::amin;
using genie::backend::test_utils::set_by_mask;
using genie::backend::test_utils::argsort;
using genie::backend::test_utils::create_matrix;
using genie::backend::test_utils::random_matrix;
using genie::backend::test_utils::permutation;

template <typename E>
inline size_t get_nrows(const E& e) { return get_mat_shape(e, 0); }

template <typename E>
inline size_t get_ncols(const E& e) { return get_mat_shape(e, 1); }

template <typename E, typename V>
inline void set_element(E& e, size_t i, size_t j, V v) { set_mat_element(e, i, j, v); }

} // namespace genie::genotype

#endif // GENIE_GENOTYPE_TEST_HELPERS_H
