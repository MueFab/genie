#include <gtest/gtest.h>
#include <util/sam-file-reader.h>
#include <iostream>
#include "helpers.h"

TEST(SamFileReader, Simple) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    util::SamFileReader reader(gitRootDir + "/resources/test-files/sam/four-reads-with-header.sam");
    std::list<util::SamRecord> records;

    EXPECT_EQ(reader.header,
              "@HD\tVN:1.3\tSO:coordinate\n@PG\tID:Illumina.SecondaryAnalysis.SortedToBamConverter\n@SQ\tSN:EcoliDH10B."
              "fa\tLN:4686137\tM5:28d8562f2f99c047d792346835b20031\n@RG\tID:_5_1\tPL:ILLUMINA\tSM:DH10B_Sample1\n");

    reader.readRecords(4, &records);

    EXPECT_EQ(records.size(), 4);
    EXPECT_EQ(records.front().qname, "_5:1:1:23848:21362");
    EXPECT_EQ(records.front().flag, 99);
    EXPECT_EQ(records.front().rname, "EcoliDH10B.fa");
    EXPECT_EQ(records.front().pos, 1);
    EXPECT_EQ(records.front().mapq, 254);
    EXPECT_EQ(records.front().cigar, "150M");
    EXPECT_EQ(records.front().rnext, "=");
    EXPECT_EQ(records.front().pnext, 224);
    EXPECT_EQ(records.front().tlen, 373);
    EXPECT_EQ(records.front().seq,
              "AGCTTTTCATTCTGACTGCAACGGGCAATATGTCTCTGTGTGGATTAAAAAAAGAGTGTCTGATAGCAGCTTCTGAACTGGTTACCTGCCGTGAGTAAATTAAA"
              "ATTTTATTGACTTAGGTCACTAAATACTTTAACCAATATAGGCATA");
    EXPECT_EQ(records.front().qual,
              "@@CDFFFFHHHHHJJJJIJJJJJJJJJJIJIJFIGIJGGGDHIGIIIJEIJGJJBHAGEHHHGHHFFFFFEEEDEEEDDDDCDDDDDDDDD?BDDCCDDEED:"
              "ACEDDDEDDCDDDDDDD>CDCCDCDDEDDDDDCDCD@CCDCDCCCDD");
    EXPECT_EQ(records.front().opt, "RG:Z:_5_1\tBC:Z:1\tXD:Z:150\tSM:i:833\tNM:i:0\tAS:i:1610");
    EXPECT_EQ(records.back().qname, "_5:1:10:7990:17938");
    EXPECT_EQ(records.back().flag, 145);
    EXPECT_EQ(records.back().rname, "EcoliDH10B.fa");
    EXPECT_EQ(records.back().pos, 3);
    EXPECT_EQ(records.back().mapq, 254);
    EXPECT_EQ(records.back().cigar, "150M");
    EXPECT_EQ(records.back().rnext, "=");
    EXPECT_EQ(records.back().pnext, 4685941);
    EXPECT_EQ(records.back().tlen, 4686090);
    EXPECT_EQ(records.back().seq,
              "CTTTTGATTCTGACTGCAACGGGCAATATGTCTCTGTGTGGATTAAAAAAAGAGTGTCTGATAGCAGCTTCTGAACTGGTTACCTGCCGTGAGTAAATTAAAAT"
              "TTTATTGACTTAGGTCACTAAATACTTTAACCAATATAGGCATACG");
    EXPECT_EQ(records.back().qual,
              ":23>>:::CDCDC>@:@>?<59C>4:>4:4(3CACA@@CAA@@CCBDDDDCA:CA??>A>CCCCADFFECDC:FEEAEEA@GC="
              "GIIFJJJJJJIJIJJJJJJJJJJIIGIGHGJJJJJJJJGJJJJHGIJJJJIHJHHHHGFFFFF@@C");
    EXPECT_EQ(records.back().opt, "RG:Z:_5_1\tBC:Z:1\tXD:Z:150\tSM:i:831\tNM:i:0");
}

TEST(SamFileReader, SimpleNoHeader) {  // NOLINT(cert-err-cpp)

    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    util::SamFileReader reader(gitRootDir + "/resources/test-files/sam/four-reads-without-header.sam");
    std::list<util::SamRecord> records;
    EXPECT_EQ(reader.header, "");
}

TEST(SamFileReader, BlankLine) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    util::SamFileReader reader(gitRootDir + "/resources/test-files/sam/extraneous-blank-line.sam");

    std::list<util::SamRecord> records;

    EXPECT_THROW(reader.readRecords(4, &records), std::invalid_argument);
}

TEST(SamFileReader, Truncated) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    util::SamFileReader reader(gitRootDir + "/resources/test-files/sam/truncated.sam");

    std::list<util::SamRecord> records;
    reader.readRecords(4, &records);

    EXPECT_EQ(records.back().qname, "_5:1:10:7990:17938");
    EXPECT_EQ(records.back().flag, 145);
    EXPECT_EQ(records.back().rname, "EcoliDH10B.fa");
    EXPECT_EQ(records.back().pos, 3);
    EXPECT_EQ(records.back().mapq, 254);
    EXPECT_EQ(records.back().cigar, "150M");
    EXPECT_EQ(records.back().rnext, "=");
    EXPECT_EQ(records.back().pnext, 4685941);
    EXPECT_EQ(records.back().tlen, 4686090);
    EXPECT_EQ(records.back().seq,
              "CTTTTGATTCTGACTGCAACGGGCAATATGTCTCTGTGTGGATTAAAAAAAGAGTGTCTGATAGCAGCTTCTGAACTGGTTACCTGCCGTGAGTAAATTAAAAT"
              "TTTATTGACTTAGGTCACTAAATACTTTAACCAATATAGGCAT");
    // The behavior of the next two lines makes very little sense.
    //    EXPECT_EQ(records.back().qual, "");
    //    EXPECT_EQ(records.back().opt, "");
}
