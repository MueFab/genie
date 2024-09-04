//
// Created by abder on 05.02.2021.
//
//#include <gtest/gtest.h>
#include "helpers.h"
#include <fstream>
#include <genie/format/mpegg_p1/dataset/dataset.h>

#include <genie/util/bit-reader.h>
#include <genie/util/bit-writer.h>

TEST(dataset, roundtrip) {  // NOLINT(cert-err-cpp)

std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
ds = Dataset(0, 2);
std::ofstream output_f(gitRootDir + "/data/sam/dataset.cc");
auto writer = genie::util::BitWriter(output_f);
ds.write(writer);


std::ifstream input_f(gitRootDir + "/data/sam/dataset.cc");
auto reader = genie::util::BitReader(input_f);
recon_ds = Dataset(reader);

EXPECT_EQ(ds.getID(), recon_ds.getID());

}

