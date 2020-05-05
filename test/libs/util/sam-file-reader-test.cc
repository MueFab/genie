#include <genie/format/sam/reader.h>
#include <gtest/gtest.h>
#include "helpers.h"

#include <fstream>
#include <list>
#include <genie/format/sam/record.h>
#include <genie/format/sam/importer.h>
#include <map>
#include <vector>
#include <genie/util/exceptions.h>

#include <genie/format/sam/reader.h>
#include <genie/format/sam/record.h>

TEST(SamRecord, Simple) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    std::ifstream f(gitRootDir + "/data/sam/four-reads-with-header.sam");
    genie::format::sam::Reader reader(f);

//    EXPECT_EQ(reader.header,
//              "@HD\tVN:1.3\tSO:coordinate\n@PG\tID:Illumina.SecondaryAnalysis.SortedToBamConverter\n@SQ\tSN:EcoliDH10B."
//              "fa\tLN:4686137\tM5:28d8562f2f99c047d792346835b20031\n@RG\tID:_5_1\tPL:ILLUMINA\tSM:DH10B_Sample1\n");

    std::string line;
    std::getline(f, line);
    genie::format::sam::Record rec(line);

    EXPECT_EQ(rec.getQname(), "_5:1:1:23848:21362");
    EXPECT_EQ(rec.getFlags(), 99);
    EXPECT_EQ(rec.getRname(), "EcoliDH10B.fa");
    EXPECT_EQ(rec.getPos(), 1);
    EXPECT_EQ(rec.getMapQ(), 254);
    EXPECT_EQ(rec.getCigar(), "150M");
    EXPECT_EQ(rec.getRnext(), "=");
    EXPECT_EQ(rec.getPnext(), 224);
    EXPECT_EQ(rec.getTlen(), 373);
    EXPECT_EQ(rec.getSeq(),
              "AGCTTTTCATTCTGACTGCAACGGGCAATATGTCTCTGTGTGGATTAAAAAAAGAGTGTCTGATAGCAGCTTCTGAACTGGTTACCTGCCGTGAGTAAATTAAA"
              "ATTTTATTGACTTAGGTCACTAAATACTTTAACCAATATAGGCATA");
    EXPECT_EQ(rec.getQual(),
              "@@CDFFFFHHHHHJJJJIJJJJJJJJJJIJIJFIGIJGGGDHIGIIIJEIJGJJBHAGEHHHGHHFFFFFEEEDEEEDDDDCDDDDDDDDD?BDDCCDDEED:"
              "ACEDDDEDDCDDDDDDD>CDCCDCDDEDDDDDCDCD@CCDCDCCCDD");
//    EXPECT_EQ(records.front().opt, "RG:Z:_5_1\tBC:Z:1\tXD:Z:150\tSM:i:833\tNM:i:0\tAS:i:1610");

    for (auto i = 0; i < 3; i++){
        std::getline(f, line);
    }
    genie::format::sam::Record lastRec(line);
    EXPECT_EQ(lastRec.getQname(), "_5:1:10:7990:17938");
    EXPECT_EQ(lastRec.getFlags(), 145);
    EXPECT_EQ(lastRec.getRname(), "EcoliDH10B.fa");
    EXPECT_EQ(lastRec.getPos(), 3);
    EXPECT_EQ(lastRec.getMapQ(), 254);
    EXPECT_EQ(lastRec.getCigar(), "150M");
    EXPECT_EQ(lastRec.getRnext(), "=");
    EXPECT_EQ(lastRec.getPnext(), 4685941);
    EXPECT_EQ(lastRec.getTlen(), 4686090);
    EXPECT_EQ(lastRec.getSeq(),
              "CTTTTGATTCTGACTGCAACGGGCAATATGTCTCTGTGTGGATTAAAAAAAGAGTGTCTGATAGCAGCTTCTGAACTGGTTACCTGCCGTGAGTAAATTAAAAT"
              "TTTATTGACTTAGGTCACTAAATACTTTAACCAATATAGGCATACG");
    // TODO: Use string literal? (Yeremia)
    EXPECT_EQ(lastRec.getQual(),
              ":23>>:::CDCDC>@:@>?<59C>4:>4:4(3CACA@@CAA@@CCBDDDDCA:CA??>A>CCCCADFFECDC:FEEAEEA@GC="
              "GIIFJJJJJJIJIJJJJJJJJJJIIGIGHGJJJJJJJJGJJJJHGIJJJJIHJHHHHGFFFFF@@C");
//    EXPECT_EQ(records.back().opt, "RG:Z:_5_1\tBC:Z:1\tXD:Z:150\tSM:i:831\tNM:i:0");
}

//TODO (Yeremia): Ask whether truncated sam record should be supported
#if false
 TEST(SamFileReader, Truncated) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    std::string line;
    std::ifstream f(gitRootDir + "/data/sam/truncated.sam");
    genie::format::sam::Reader reader(f);

    for (auto i = 0; i < 4; i++){
        std::getline(f, line);
    }


    ASSERT_THROW(genie::format::sam::Record rec(line), genie::util::RuntimeException);
    EXPECT_EQ(rec.getQname(), "_5:1:10:7990:17938");
    EXPECT_EQ(rec.getFlags(), 145);
    EXPECT_EQ(rec.getRname(), "EcoliDH10B.fa");
    EXPECT_EQ(rec.getPos(), 3);
    EXPECT_EQ(rec.getMapQ(), 254);
    EXPECT_EQ(rec.getCigar(), "150M");
    EXPECT_EQ(rec.getRnext(), "=");
    EXPECT_EQ(rec.getPnext(), 4685941);
    EXPECT_EQ(rec.getTlen(), 4686090);
    EXPECT_EQ(rec.getSeq(),
              "CTTTTGATTCTGACTGCAACGGGCAATATGTCTCTGTGTGGATTAAAAAAAGAGTGTCTGATAGCAGCTTCTGAACTGGTTACCTGCCGTGAGTAAATTAAAAT"
              "TTTATTGACTTAGGTCACTAAATACTTTAACCAATATAGGCAT");
    // The behavior of the next two lines makes very little sense.
    EXPECT_EQ(rec.getQual(), "");
    //    EXPECT_EQ(records.back().opt, "");
}
#endif

// TODO (Yeremia): Remove or fix this example. SAM does not allow record to use alternative alias of reference.
#if false
TEST(SamFileReader, PairReferenceWithAlternativeAlias) {  // NOLINT(cert-err-cpp)
    // Test Reference with alternative alias

    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::ifstream f(gitRootDir + "/data/sam/sample01.sam");
    genie::format::sam::Reader reader(f);

    std::vector<genie::format::sam::Record> recs;
    reader.read(1, recs);
//    std::list<genie::format::sam::Record> unmappedRead, read1, read2;
//    EXPECT_NO_THROW(EXPECT_EQ(reader.getSortedTemplate(unmappedRead, read1, read2), true));
}
#endif

//TEST(SamReader, SingleEndUnmapped) {  // NOLINT(cert-err-cpp)
//    // Test sam file containing alignments with invalid flags
//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//    std::ifstream f(gitRootDir + "/data/sam/sample06.sam");
//    genie::format::sam::Reader reader(f);
//
////    genie::format::sam::Record rec;
////    reader.read(rec);
////
////    genie::format::sam::Template t(std::move(rec));
////
////    EXPECT_EQ(7, 7);
//
//    std::vector<genie::format::sam::Record> recs;
//    genie::format::sam::TemplateGroup tg;
//
//    reader.readRecords(10, recs);
//
//    tg.addRecords(recs);
//
//    EXPECT_EQ(7, 7);
//}
//
//TEST(SamReader, PairedEndHeaderInvalid) {  // NOLINT(cert-err-cpp)
//    // Test sam file with invalid SAM header tag
//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//    std::ifstream f(gitRootDir + "/data/sam/sample02.sam");
//    ASSERT_THROW(genie::format::sam::Reader reader(f);, genie::util::RuntimeException);
//}
//
//TEST(SamReader, PairedEndReferenceNotExists) {  // NOLINT(cert-err-cpp)
//    // Test sam file with record, which reference does not exist in header
//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//    std::ifstream f(gitRootDir + "/data/sam/sample03.sam");
//    genie::format::sam::Reader reader(f);
//    ASSERT_THROW(reader.read(10), genie::util::RuntimeException);
//}
//
//TEST(SamReader, PairedEndInvalidFlags) {  // NOLINT(cert-err-cpp)
//    // Test sam file containing alignments with invalid flags
//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//    {
//        // Paired-end but no MultiSeg flag
//        std::ifstream f(gitRootDir + "/data/sam/sample04.sam");
//        genie::format::sam::Reader reader(f);
//        ASSERT_THROW(reader.read(10), genie::util::RuntimeException);
//    }
//    {
//        // FirstSegment & LastSegment
//        std::ifstream f(gitRootDir + "/data/sam/sample05.sam");
//        genie::format::sam::Reader reader(f);
//        ASSERT_THROW(reader.read(10), genie::util::RuntimeException);
//    }
//}
//
//TEST(SamImporter, PairedEndMultiAlignment) {  // NOLINT(cert-err-cpp)
//    std::vector<std::string> fnames = {
//            "K562_cytosol_LID8465_GEM_v3.sam",      // 0
//            "K562_cytosol_LID8465_TopHat_v2.sam",   // 1
//            "simulation.1.homoINDELs.homoCEUsnps.reads2.fq.sam.samelength.sam", // 2
//            "SRR327342.sam" // 3
//    };
//
//    std::ifstream f("/phys/ssd/tmp/truncated_sam/" + fnames[0]);
//
//    std::list<genie::format::sam::Record> unmappedRead, read1, read2;
//    genie::format::sam::Reader reader(f);
//
//    reader.read(1);
//
//    if (reader.getSortedTemplate(unmappedRead, read1, read2)){
//        EXPECT_EQ(unmappedRead.size(), 0);
//        EXPECT_EQ(read1.size(), 7);
//        EXPECT_EQ(read2.size(), 7);
////        auto mpegRec = genie::format::sam::Importer::convert(0, std::move(read1), &read2);
//    }
//
//    EXPECT_EQ(reader.getSortedTemplate(unmappedRead, read1, read2), false);
//    EXPECT_EQ(unmappedRead.size(), 0);
//    EXPECT_EQ(read1.size(), 0);
//    EXPECT_EQ(read2.size(), 0);
//}

//TEST(SamImporter, TruncatedSAM) {  // NOLINT(cert-err-cpp)
//    std::vector<std::string> fnames = {
//            "K562_cytosol_LID8465_GEM_v3.sam",      // 0
//            "K562_cytosol_LID8465_TopHat_v2.sam",   // 1
//            "simulation.1.homoINDELs.homoCEUsnps.reads2.fq.sam.samelength.sam", // 2
//            "SRR327342.sam" // 3
//    };
//
//    std::ifstream f("/phys/ssd/tmp/truncated_sam/" + fnames[0]);
//
//    genie::format::sam::Importer samImporter(512, f);
//
//}

TEST(SamFileReader, main) {  // NOLINT(cert-err-cpp)
    std::vector<std::string> fnames = {
            "9799_7#3.sam",  // 0 - TODO: As an example for header
            "9827_2#49.sam", // 1 - OK
            "dm3PacBio.sam", // 2 - OK
            "ERR174310.aln.sort.dupmark.rg.recal.sam", // 3 - OK
            "ERR174310.mult.sorted.sam",    // 4 - TODO: ERROR due to invalid SAM Record
            "G15511.HCC1143.1.sam",         // 5 - TODO: As an example for header
            "HCC1954.mix1.n80t20.sam",      // 6 - TODO: As an example for header
            "K562_cytosol_LID8465_GEM_v3.sam",      // 7 - pair multi_alignment - TODO: Create smaller data
            "K562_cytosol_LID8465_TopHat_v2.sam",   // 8 - OK
            "MiSeq_Ecoli_DH10B_110721_PF.sam",      // 9 - OK
            "NA12877_S1.sam",   // 10 - TODO: tag ID PP
            "NA12878_S1.sam",   // 11
            "NA12878-Rep-1_S1_L001_2.aln.sort.dupmark.rg.sam",  // 12 - TODO: tag PL
            "NA12878.pacbio.bwa-sw.20140202.sam",               // 13
            "NA12879_S1.sam",         // 14 - (incomplete) pair - TODO: test
            "NA12882_S1.sam",         // 15 - pair - TODO: tag ID, PP
            "NA12890_S1.sam",         // 16 - incomplete pairs - ok
            "NA21144.chrom11.ILLUMINA.bwa.GIH.low_coverage.20130415.sam", // 17 - TODO: tag VN
            "sample-2-10_sorted.sam", // 18 - single - ok
            "sample-2-11_sorted.sam", // 19 - single - ok
            "sample-2-12_sorted.sam", // 20 - single - ok
            "simulation.1.homoINDELs.homoCEUsnps.reads2.fq.sam.samelength.sam", // 21 - pair - ok
            "SRR327342.sam"           // 22 - single unmapped - TODO: Check converter for unmapped
    };
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

//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//    std::ifstream f("/home/adhisant/tmp/data/SAM_TEST/" + fnames[7]);
//    std::ifstream f("/phys/ssd/tmp/truncated_sam/" + fnames[7]);
    std::ifstream f("/home/sxperfect/tmp/sam/truncated_sam/" + fnames[7]);

//    std::ifstream f("/home/adhisant/tmp/data/testdata/" + fnames2[3]);

    UTILS_DIE_IF(f.fail(), "File not found!");
    std::vector<genie::format::sam::Record> recs;
    genie::format::sam::ReadTemplateGroup tg;

    genie::format::sam::Reader reader(f);
    reader.readRecords(10000, recs);

    tg.addRecords(recs);

    tg.resetCounter();

    std::list<genie::format::sam::ReadTemplate> ts;
    tg.getTemplates(ts, 20);
    UTILS_DIE_IF(!(ts.begin())->isValid(), "invalid");

    EXPECT_EQ(7, 7);

//    std::list<genie::format::sam::Record> unmappedRead, read1, read2;
//    genie::format::sam::Reader reader(f);
//    reader.read(1);
//
//    auto success = reader.getSortedRecord(unmappedRead, read1, read2);
//
//    std::string x("AAA");
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

//
// TEST(SamFileReader, SimpleNoHeader) {  // NOLINT(cert-err-cpp)
//
//    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
//    genie::util::SamFileReader reader(gitRootDir + "/data/sam/four-reads-without-header.sam");
//    std::list<format::sam::SamRecord> records;
//    EXPECT_EQ(reader.header, "");
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

