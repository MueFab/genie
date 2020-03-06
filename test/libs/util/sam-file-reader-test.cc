#include <genie/format/sam/reader.h>
#include <gtest/gtest.h>
#include <iostream>
#include "helpers.h"

#include <fstream>
#include <list>
#include <genie/format/sam/record.h>
#include <genie/format/sam/importer.h>
#include <map>
#include <vector>
#include <regex>
#include <genie/util/exceptions.h>

#include <genie/format/sam/reader.h>

TEST(SamFileReader, ReferenceWithAlias) {  // NOLINT(cert-err-cpp)
    // Test Reference with alternative alias

    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::ifstream f(gitRootDir + "/data/sam/sample01.sam");
    genie::format::sam::Reader reader(f);
    EXPECT_NO_THROW(reader.read());
}

TEST(SamFileReader, HeaderInvalid) {  // NOLINT(cert-err-cpp)
    // Test sam file with invalid SAM header tag
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::ifstream f(gitRootDir + "/data/sam/sample02.sam");
    ASSERT_THROW(genie::format::sam::Reader reader(f);, genie::util::RuntimeException);
}

TEST(SamFileReader, IncompleteReference) {  // NOLINT(cert-err-cpp)
    // Test sam file with record, which reference does not exist in header
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::ifstream f(gitRootDir + "/data/sam/sample03.sam");
    ASSERT_THROW(genie::format::sam::Reader reader(f);, genie::util::RuntimeException);
}

TEST(SamFileReader, main) {  // NOLINT(cert-err-cpp)
//    std::vector<std::string> fnames = {
//            "9799_7#3.sam",  // 0 - TODO: As an example for header
//            "9827_2#49.sam", // 1 - OK
//            "dm3PacBio.sam", // 2 - OK
//            "ERR174310.aln.sort.dupmark.rg.recal.sam", // 3 - OK
//            "ERR174310.mult.sorted.sam",    // 4 - TODO: ERROR due to invalid SAM Record
//            "G15511.HCC1143.1.sam",         // 5 - TODO: As an example for header
//            "HCC1954.mix1.n80t20.sam",      // 6 - TODO: As an example for header
//            "K562_cytosol_LID8465_GEM_v3.sam",      // 7 - pair multi_allignment - TODO: Create smaller data
//            "K562_cytosol_LID8465_TopHat_v2.sam",   // 8 - OK
//            "MiSeq_Ecoli_DH10B_110721_PF.sam",      // 9 - OK
//            "NA12877_S1.sam",   // 10 - TODO: tag ID PP
//            "NA12878_S1.sam",   // 11
//            "NA12878-Rep-1_S1_L001_2.aln.sort.dupmark.rg.sam",  // 12 - TODO: tag PL
//            "NA12878.pacbio.bwa-sw.20140202.sam",               // 13
//            "NA12879_S1.sam",         // 14 - (incomplete) pair - TODO: test
//            "NA12882_S1.sam",         // 15 - pair - TODO: tag ID, PP
//            "NA12890_S1.sam",         // 16 - incomplete pairs - ok
//            "NA21144.chrom11.ILLUMINA.bwa.GIH.low_coverage.20130415.sam", // 17 - TODO: tag VN
//            "sample-2-10_sorted.sam", // 18 - single - ok
//            "sample-2-11_sorted.sam", // 19 - single - ok
//            "sample-2-12_sorted.sam", // 20 - single - ok
//            "simulation.1.homoINDELs.homoCEUsnps.reads2.fq.sam.samelength.sam", // 21 - pair - ok
//            "SRR327342.sam"           // 22 - single unmapped - TODO: Check converter for unmapped
//    };
//
//    std::vector<std::string> fnames2 = {
//            "aux#aux.sam",          // 0
//            "aux#special.sam",      // 1
//            "c1#bounds.sam",        // 2 - single mapped
//            "c1#clip.sam",          // 3 -
//            "c1#pad1.sam",          // 4
//            "c1#pad2.sam",          // 5
//            "c1#pad3.sam",          // 6
//            "ce#1.sam",             // 7
//            "ce#2.sam",             // 8
//            "ce#5b.sam",            // 9
//            "ce#5.sam",             // 10
//            "ce#large_seq.sam",     // 11
//            "ce#tag_depadded.sam",  // 12
//            "ce#tag_padded.sam",    // 13
//            "ce#unmap1.sam",        // 14
//            "ce#unmap2.sam",        // 15
//            "ce#unmap.sam",         // 16
//            "list.txt",             // 17
//            "small.sam",            // 18
//            "text.sam",             // 19
//            "xx#blank.sam",         // 20
//            "xx#large_aux2.sam",    // 21
//            "xx#large_aux.sam",     // 22
//            "xx#minimal.sam",       // 23
//            "xx#pair.sam",          // 24
//            "xx#rg.sam",            // 25
//            "xx#triplet.sam",       // 26
//            "xx#unsorted.sam",      // 27
//            "mappingOperations.sam",// 28
//            "readNameTest1.sam",    // 29
//            "readNameTest2.sam",    // 30
//            "readNameTest3.sam",    // 31
//            "readNameTest4.sam",    // 32
//            "readNameTest5.sam"     // 33
//    };
//
////    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//    std::ifstream f("/home/adhisant/tmp/data/SAM_TEST/" + fnames[7]);
////    std::ifstream f("/home/adhisant/tmp/data/testdata/" + fnames2[3]);
//
//    genie::format::sam::Reader reader(f);
//
//    std::vector<genie::format::sam::Record> recs;
//    reader.read();
//
//
//    genie::format::sam::Importer sam((std::size_t) 10000, f);
//
//    sam.pump(size_t(0));
}





//TEST(SamFileReader, Simple) {  // NOLINT(cert-err-cpp)
//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
////    genie::util::SamFileReader reader(gitRootDir + "/data/sam/four-reads-with-header.sam");
//
////    std::ifstream f(gitRootDir + "/data/sam/four-reads-with-header.sam");
//    std::ifstream f("/home/adhisant/tmp/data/SAM/testfile.sam");
//
////    genie::format::sam::Importer sam((std::size_t) 5, f);
//
//    std::map<std::string, std::vector<int>> alignment_table;
//    std::list<genie::format::sam::Record> records;
//
//    std::regex REGEX_QNAME("[!-?A-~]{1,254}");
//    std::string qname;
//
//    while( f.peek() == '@') {
//        f.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//    }
//
//    while( f.peek() != EOF )
//    {
//        int pos = f.tellg();
//
//        std::getline(f, qname, '\t');
//
//        UTILS_DIE_IF(!std::regex_match(qname, REGEX_QNAME), "Invalid QNAME");
//
//        if (alignment_table.find(qname) == alignment_table.end()) {
//            alignment_table.emplace(qname, std::vector<int>());
//        }
//
////        alignment_table[qname].emplace_back(std::move(pos + qname.length() + 1));
//        alignment_table[qname].emplace_back(std::move(pos));
//
//        std::getline(f, qname);
//    }
//
//    std::cout << "complete" << std::endl;
//
//    for (auto qname_iter = alignment_table.begin(); qname_iter != alignment_table.end(); qname_iter++) {
//        qname = qname_iter->first;
//        for (auto entry_iter = qname_iter->second.begin(); entry_iter != qname_iter->second.end(); entry_iter++) {
//            f.seekg(*entry_iter);
//            std::string alignment_str;
//            std::getline(f, alignment_str, '\n');
//
//            records.emplace_back(genie::format::sam::Record(alignment_str));
//        }
//    }
//}

// TEST(SamFileReader, Simple) {  // NOLINT(cert-err-cpp)
//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//    genie::util::SamFileReader reader(gitRootDir + "/data/sam/four-reads-with-header.sam");
//    std::list<format::sam::SamRecord> records;
//
//    EXPECT_EQ(reader.header,
//              "@HD\tVN:1.3\tSO:coordinate\n@PG\tID:Illumina.SecondaryAnalysis.SortedToBamConverter\n@SQ\tSN:EcoliDH10B."
//              "fa\tLN:4686137\tM5:28d8562f2f99c047d792346835b20031\n@RG\tID:_5_1\tPL:ILLUMINA\tSM:DH10B_Sample1\n");
//
//    reader.readRecords(4, &records);
//
//    EXPECT_EQ(records.size(), 4);
//    EXPECT_EQ(records.front().qname, "_5:1:1:23848:21362");
//    EXPECT_EQ(records.front().flag, 99);
//    EXPECT_EQ(records.front().rname, "EcoliDH10B.fa");
//    EXPECT_EQ(records.front().pos, 1);
//    EXPECT_EQ(records.front().mapq, 254);
//    EXPECT_EQ(records.front().cigar, "150M");
//    EXPECT_EQ(records.front().rnext, "=");
//    EXPECT_EQ(records.front().pnext, 224);
//    EXPECT_EQ(records.front().tlen, 373);
//    EXPECT_EQ(records.front().seq,
//              "AGCTTTTCATTCTGACTGCAACGGGCAATATGTCTCTGTGTGGATTAAAAAAAGAGTGTCTGATAGCAGCTTCTGAACTGGTTACCTGCCGTGAGTAAATTAAA"
//              "ATTTTATTGACTTAGGTCACTAAATACTTTAACCAATATAGGCATA");
//    EXPECT_EQ(records.front().qual,
//              "@@CDFFFFHHHHHJJJJIJJJJJJJJJJIJIJFIGIJGGGDHIGIIIJEIJGJJBHAGEHHHGHHFFFFFEEEDEEEDDDDCDDDDDDDDD?BDDCCDDEED:"
//              "ACEDDDEDDCDDDDDDD>CDCCDCDDEDDDDDCDCD@CCDCDCCCDD");
//    EXPECT_EQ(records.front().opt, "RG:Z:_5_1\tBC:Z:1\tXD:Z:150\tSM:i:833\tNM:i:0\tAS:i:1610");
//    EXPECT_EQ(records.back().qname, "_5:1:10:7990:17938");
//    EXPECT_EQ(records.back().flag, 145);
//    EXPECT_EQ(records.back().rname, "EcoliDH10B.fa");
//    EXPECT_EQ(records.back().pos, 3);
//    EXPECT_EQ(records.back().mapq, 254);
//    EXPECT_EQ(records.back().cigar, "150M");
//    EXPECT_EQ(records.back().rnext, "=");
//    EXPECT_EQ(records.back().pnext, 4685941);
//    EXPECT_EQ(records.back().tlen, 4686090);
//    EXPECT_EQ(records.back().seq,
//              "CTTTTGATTCTGACTGCAACGGGCAATATGTCTCTGTGTGGATTAAAAAAAGAGTGTCTGATAGCAGCTTCTGAACTGGTTACCTGCCGTGAGTAAATTAAAAT"
//              "TTTATTGACTTAGGTCACTAAATACTTTAACCAATATAGGCATACG");
//    EXPECT_EQ(records.back().qual,
//              ":23>>:::CDCDC>@:@>?<59C>4:>4:4(3CACA@@CAA@@CCBDDDDCA:CA??>A>CCCCADFFECDC:FEEAEEA@GC="
//              "GIIFJJJJJJIJIJJJJJJJJJJIIGIGHGJJJJJJJJGJJJJHGIJJJJIHJHHHHGFFFFF@@C");
//    EXPECT_EQ(records.back().opt, "RG:Z:_5_1\tBC:Z:1\tXD:Z:150\tSM:i:831\tNM:i:0");
//}
//
// TEST(SamFileReader, SimpleNoHeader) {  // NOLINT(cert-err-cpp)
//
//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//    genie::util::SamFileReader reader(gitRootDir + "/data/sam/four-reads-without-header.sam");
//    std::list<format::sam::SamRecord> records;
//    EXPECT_EQ(reader.header, "");
//}

// TEST(SamFileReader, Simple) {  // NOLINT(cert-err-cpp)
//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//    genie::util::SamFileReader reader(gitRootDir + "/data/sam/four-reads-with-header.sam");
//    std::list<format::sam::SamRecord> records;
//
//    EXPECT_EQ(reader.header,
//              "@HD\tVN:1.3\tSO:coordinate\n@PG\tID:Illumina.SecondaryAnalysis.SortedToBamConverter\n@SQ\tSN:EcoliDH10B."
//              "fa\tLN:4686137\tM5:28d8562f2f99c047d792346835b20031\n@RG\tID:_5_1\tPL:ILLUMINA\tSM:DH10B_Sample1\n");
//
//    reader.readRecords(4, &records);
//
//    EXPECT_EQ(records.size(), 4);
//    EXPECT_EQ(records.front().qname, "_5:1:1:23848:21362");
//    EXPECT_EQ(records.front().flag, 99);
//    EXPECT_EQ(records.front().rname, "EcoliDH10B.fa");
//    EXPECT_EQ(records.front().pos, 1);
//    EXPECT_EQ(records.front().mapq, 254);
//    EXPECT_EQ(records.front().cigar, "150M");
//    EXPECT_EQ(records.front().rnext, "=");
//    EXPECT_EQ(records.front().pnext, 224);
//    EXPECT_EQ(records.front().tlen, 373);
//    EXPECT_EQ(records.front().seq,
//              "AGCTTTTCATTCTGACTGCAACGGGCAATATGTCTCTGTGTGGATTAAAAAAAGAGTGTCTGATAGCAGCTTCTGAACTGGTTACCTGCCGTGAGTAAATTAAA"
//              "ATTTTATTGACTTAGGTCACTAAATACTTTAACCAATATAGGCATA");
//    EXPECT_EQ(records.front().qual,
//              "@@CDFFFFHHHHHJJJJIJJJJJJJJJJIJIJFIGIJGGGDHIGIIIJEIJGJJBHAGEHHHGHHFFFFFEEEDEEEDDDDCDDDDDDDDD?BDDCCDDEED:"
//              "ACEDDDEDDCDDDDDDD>CDCCDCDDEDDDDDCDCD@CCDCDCCCDD");
//    EXPECT_EQ(records.front().opt, "RG:Z:_5_1\tBC:Z:1\tXD:Z:150\tSM:i:833\tNM:i:0\tAS:i:1610");
//    EXPECT_EQ(records.back().qname, "_5:1:10:7990:17938");
//    EXPECT_EQ(records.back().flag, 145);
//    EXPECT_EQ(records.back().rname, "EcoliDH10B.fa");
//    EXPECT_EQ(records.back().pos, 3);
//    EXPECT_EQ(records.back().mapq, 254);
//    EXPECT_EQ(records.back().cigar, "150M");
//    EXPECT_EQ(records.back().rnext, "=");
//    EXPECT_EQ(records.back().pnext, 4685941);
//    EXPECT_EQ(records.back().tlen, 4686090);
//    EXPECT_EQ(records.back().seq,
//              "CTTTTGATTCTGACTGCAACGGGCAATATGTCTCTGTGTGGATTAAAAAAAGAGTGTCTGATAGCAGCTTCTGAACTGGTTACCTGCCGTGAGTAAATTAAAAT"
//              "TTTATTGACTTAGGTCACTAAATACTTTAACCAATATAGGCATACG");
//    EXPECT_EQ(records.back().qual,
//              ":23>>:::CDCDC>@:@>?<59C>4:>4:4(3CACA@@CAA@@CCBDDDDCA:CA??>A>CCCCADFFECDC:FEEAEEA@GC="
//              "GIIFJJJJJJIJIJJJJJJJJJJIIGIGHGJJJJJJJJGJJJJHGIJJJJIHJHHHHGFFFFF@@C");
//    EXPECT_EQ(records.back().opt, "RG:Z:_5_1\tBC:Z:1\tXD:Z:150\tSM:i:831\tNM:i:0");
//}
//
// TEST(SamFileReader, SimpleNoHeader) {  // NOLINT(cert-err-cpp)
//
//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//    genie::util::SamFileReader reader(gitRootDir + "/data/sam/four-reads-without-header.sam");
//    std::list<format::sam::SamRecord> records;
//    EXPECT_EQ(reader.header, "");
//}
//
// TEST(SamFileReader, BlankLine) {  // NOLINT(cert-err-cpp)
//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//
//    genie::util::SamFileReader reader(gitRootDir + "/data/sam/extraneous-blank-line.sam");
//
//    std::list<format::sam::SamRecord> records;
//
//    EXPECT_THROW(reader.readRecords(4, &records), std::invalid_argument);
//}
//
// TEST(SamFileReader, Truncated) {  // NOLINT(cert-err-cpp)
//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//
//    genie::util::SamFileReader reader(gitRootDir + "/data/sam/truncated.sam");
//
//    std::list<format::sam::SamRecord> records;
//    reader.readRecords(4, &records);
//
//    EXPECT_EQ(records.back().qname, "_5:1:10:7990:17938");
//    EXPECT_EQ(records.back().flag, 145);
//    EXPECT_EQ(records.back().rname, "EcoliDH10B.fa");
//    EXPECT_EQ(records.back().pos, 3);
//    EXPECT_EQ(records.back().mapq, 254);
//    EXPECT_EQ(records.back().cigar, "150M");
//    EXPECT_EQ(records.back().rnext, "=");
//    EXPECT_EQ(records.back().pnext, 4685941);
//    EXPECT_EQ(records.back().tlen, 4686090);
//    EXPECT_EQ(records.back().seq,
//              "CTTTTGATTCTGACTGCAACGGGCAATATGTCTCTGTGTGGATTAAAAAAAGAGTGTCTGATAGCAGCTTCTGAACTGGTTACCTGCCGTGAGTAAATTAAAAT"
//              "TTTATTGACTTAGGTCACTAAATACTTTAACCAATATAGGCAT");
//    // The behavior of the next two lines makes very little sense.
//    //    EXPECT_EQ(records.back().qual, "");
//    //    EXPECT_EQ(records.back().opt, "");
//}
