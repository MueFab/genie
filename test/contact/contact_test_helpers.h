/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CONTACT_TEST_HELPERS_H
#define GENIE_CONTACT_TEST_HELPERS_H

#include "genie/contact/contact_types.h"
#include "genie/backend/test_utils.h"

namespace genie::contact {

using genie::backend::get_mat_shape;
using genie::backend::get_mat_element;
using genie::backend::set_mat_element;

using genie::backend::test_utils::PrintTo;
using genie::backend::test_utils::create_vector;
using genie::backend::test_utils::create_matrix;
using genie::backend::test_utils::sum;
using genie::backend::test_utils::cast;
using genie::backend::test_utils::sort;
using genie::backend::test_utils::equal;
using genie::backend::test_utils::all;
using genie::backend::test_utils::dims;
using genie::backend::test_utils::add_inplace;
using genie::backend::test_utils::not_equal;
using genie::backend::test_utils::get;
using genie::backend::test_utils::rows;
using genie::backend::test_utils::cols;
using genie::backend::test_utils::random_randint_vec;
using genie::backend::test_utils::random_randint_mat;
using genie::backend::test_utils::create_zeros;
using genie::backend::test_utils::create_ones;
using genie::backend::test_utils::add;
using genie::backend::test_utils::subtract;
using genie::backend::test_utils::divide;
using genie::backend::test_utils::linspace;

// Manual overloads if needed, currently delegating purely via using declarations

#define ASSERT_CM_EQUAL(a, b) ASSERT_TRUE(genie::contact::equal(a, b))
#define EXPECT_CM_EQUAL(a, b) EXPECT_TRUE(genie::contact::equal(a, b))

} // namespace genie::contact

#endif // GENIE_CONTACT_TEST_HELPERS_H
