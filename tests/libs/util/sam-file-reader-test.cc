#include <gtest/gtest.h>
#include <util/sam-file-reader.h>
#include "helpers.h"

TEST(SamFileReader, Simple) {  //NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    util::SamFileReader reader(gitRootDir + "/resources/test-files/sam/four-reads-with-header.sam");
    std::list<util::SamRecord> records;
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
    EXPECT_EQ(records.front().seq, "AGCTTTTCATTCTGACTGCAACGGGCAATATGTCTCTGTGTGGATTAAAAAAAGAGTGTCTGATAGCAGCTTCTGAACTGGTTACCTGCCGTGAGTAAATTAAAATTTTATTGACTTAGGTCACTAAATACTTTAACCAATATAGGCATA");
    EXPECT_EQ(records.front().qual, "@@CDFFFFHHHHHJJJJIJJJJJJJJJJIJIJFIGIJGGGDHIGIIIJEIJGJJBHAGEHHHGHHFFFFFEEEDEEEDDDDCDDDDDDDDD?BDDCCDDEED:ACEDDDEDDCDDDDDDD>CDCCDCDDEDDDDDCDCD@CCDCDCCCDD");
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
    EXPECT_EQ(records.back().seq, "CTTTTGATTCTGACTGCAACGGGCAATATGTCTCTGTGTGGATTAAAAAAAGAGTGTCTGATAGCAGCTTCTGAACTGGTTACCTGCCGTGAGTAAATTAAAATTTTATTGACTTAGGTCACTAAATACTTTAACCAATATAGGCATACG");
    EXPECT_EQ(records.back().qual, ":23>>:::CDCDC>@:@>?<59C>4:>4:4(3CACA@@CAA@@CCBDDDDCA:CA??>A>CCCCADFFECDC:FEEAEEA@GC=GIIFJJJJJJIJIJJJJJJJJJJIIGIGHGJJJJJJJJGJJJJHGIJJJJIHJHHHHGFFFFF@@C");
    EXPECT_EQ(records.back().opt, "RG:Z:_5_1\tBC:Z:1\tXD:Z:150\tSM:i:831\tNM:i:0");
    EXPECT_EQ(records., "");
}

TEST(SamFileReader, SimpleNoHeader) {  //NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    util::SamFileReader reader(gitRootDir + "/resources/test-files/sam/four-reads-without-header.sam");
    std::list<util::SamRecord> records;
    reader.readRecords(4, &records);

}
//TEST(FastqFileReader, FourteenRecords) {  //NOLINT(cert-err-cpp)
//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//
//    util::FastqFileReader reader(gitRootDir + "/resources/test-files/fastq/fourteen-gattaca-records.fastq");
//
//    std::vector<util::FastqRecord> records;
//    reader.readRecords(14, &records);
//
//    EXPECT_EQ(records.size(), 14);
//    EXPECT_EQ(records.front().title, "@title1");
//    EXPECT_EQ(records.front().sequence, "GATTACA");
//    EXPECT_EQ(records.front().optional, "+");
//    EXPECT_EQ(records.front().qualityScores, "QUALITY");
//
//    EXPECT_EQ(records.back().title, "@title14");
//    EXPECT_EQ(records.back().sequence, "ACATTAG");
//    EXPECT_EQ(records.back().optional, "+");
//    EXPECT_EQ(records.back().qualityScores, "YTILAUQ");
//}
//
//TEST(FastqFileReader, BlankLine) {  //NOLINT(cert-err-cpp)
//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//
//    util::FastqFileReader reader(gitRootDir + "/resources/test-files/fastq/blank-line.fastq");
//
//    std::vector<util::FastqRecord> records;
//    reader.readRecords(2, &records);
//
//    EXPECT_EQ(records.size(), 2);
//    EXPECT_EQ(records.front().title, "@title1");
//    EXPECT_EQ(records.front().sequence, "GATTACA");
//    EXPECT_EQ(records.front().optional, "+");
//    EXPECT_EQ(records.front().qualityScores, "QUALITY");
//
//    EXPECT_EQ(records.back().title, "\n");
//    EXPECT_EQ(records.back().sequence, "GATTACA");
//    EXPECT_EQ(records.back().optional, "+");
//    EXPECT_EQ(records.back().qualityScores, "QUALITY");
//}
//
//TEST(FastqFileReader, Truncated) {  //NOLINT(cert-err-cpp)
//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//
//    util::FastqFileReader reader(gitRootDir + "/resources/test-files/fastq/fourteen-gattaca-records-truncated.fastq");
//
//    std::vector<util::FastqRecord> records;
//    EXPECT_THROW(reader.readRecords(14, &records), std::runtime_error);
//
//}