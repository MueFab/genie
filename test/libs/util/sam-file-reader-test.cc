/*#include <genie/format/sam/reader.h>
#include <gtest/gtest.h>
#include "helpers.h"

#include <fstream>
#include <list>
#include <genie/format/sam/record.h>
#include <genie/format/sam/importer.h>
#include <map>
#include <vector>
#include <genie/util/runtime-exception.h>

#include <genie/format/sam/reader.h>
#include <genie/format/sam/record.h>

#include <genie/core/classifier-bypass.h>

#include <genie/core/record/alignment_split/same-rec.h>
#include <genie/core/record/alignment_split/other-rec.h>
#include <genie/core/record/alignment_split/unpaired.h>
#include <genie/core/record/alignment_external/other-rec.h>

TEST(SamRecord, ParseString) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    std::ifstream f(gitRootDir + "/data/sam/four-reads-with-header.sam");

    // Disable index to preserve the order of sam records
    genie::format::sam::Reader reader(f, false);

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
    EXPECT_EQ(lastRec.getQual(),
              ":23>>:::CDCDC>@:@>?<59C>4:>4:4(3CACA@@CAA@@CCBDDDDCA:CA\?\?>A>CCCCADFFECDC:FEEAEEA@GC="
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

#if true
TEST(SAM, SamReaderReference) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    // Valid RName
    {
        std::ifstream f(gitRootDir + "/data/sam/pair_reads.sam");
        UTILS_DIE_IF(!f.good(), "Cannot read file");

        genie::format::sam::Reader reader(f);

        auto& refs = reader.getRefs();

        std::list<std::string> lines;
        reader.read(lines);
        for (auto &line : lines){
            genie::format::sam::Record rec(line);
            EXPECT_NO_THROW(refs.at(rec.getRname()));
        }
    }
    // Invalid RName
    {
        std::ifstream f(gitRootDir + "/data/sam/invalid_rname.sam");
        UTILS_DIE_IF(!f.good(), "Cannot read file");

        genie::format::sam::Reader reader(f);
        auto& refs = reader.getRefs();

        std::list<std::string> lines;
        reader.read(lines);
        for (auto &line : lines){
            genie::format::sam::Record rec(line);
            EXPECT_ANY_THROW(refs.at(rec.getRname()));
        }
    }
}
#endif

TEST(SAM, ImporterUnmappedReads) {  // NOLINT(cert-err-cpp)
    // Test sam file containing alignments with invalid flags
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::ifstream f(gitRootDir + "/data/sam/unmapped_reads.sam");
    UTILS_DIE_IF(!f.good(), "Cannot read file");

    genie::format::sam::Reader reader(f);

    std::list<std::string> lines;
    std::list<genie::format::sam::ReadTemplate> rts;
    genie::format::sam::ReadTemplateGroup rtg;

//    genie::core::record::Chunk chunk;
    std::list<genie::core::record::Record> chunk;
    {
        while (reader.good()){
            reader.read(lines);
        }
    }

    rtg.addRecords(lines);
    lines.clear();
    rtg.getTemplates(rts);

    ASSERT_EQ(rts.size(), 5);

    for (auto & rt: rts){
        EXPECT_TRUE(rt.isValid() && rt.isUnmapped());
        // genie::format::sam::Importer::convert(chunk.getData(), rt, reader.getRefs());
//        EXPECT_EQ(chunk.getData().back().getNumberOfTemplateSegments(), 1);
//        EXPECT_EQ(chunk.getData().back().getClassID(), genie::core::record::ClassType::CLASS_I);
//        EXPECT_EQ(chunk.getData().back().getFlags(), 0);

        genie::format::sam::Importer::convert(chunk, rt, reader.getRefs());
        EXPECT_EQ(chunk.back().getNumberOfTemplateSegments(), 1);
        EXPECT_EQ(chunk.back().getClassID(), genie::core::record::ClassType::CLASS_I);
        EXPECT_EQ(chunk.back().getFlags(), 0);
    }

}

TEST(SAM, ImporterPairReadsMultipleAlignmentsSameRec) {  // NOLINT(cert-err-cpp)

    // Test sam file containing alignments with invalid flags
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::ifstream f(gitRootDir + "/data/sam/pair_reads_multi_alignments.sam");
    UTILS_DIE_IF(!f.good(), "Cannot read file");

    genie::format::sam::Reader reader(f);

    std::list<std::string> lines;
    std::list<genie::format::sam::ReadTemplate> rts;
    genie::format::sam::ReadTemplateGroup rtg;

//    genie::core::record::Chunk chunk;
    std::list<genie::core::record::Record> chunk;
    {
        while (reader.good()){
            reader.read(lines);
        }
    }

    rtg.addRecords(lines);
    lines.clear();
    rtg.getTemplates(rts);

    ASSERT_EQ(rts.size(), 1);

    genie::format::sam::Importer::convert(chunk, rts.front(), reader.getRefs());

//    EXPECT_EQ(chunk.getData().size(), 4);
    EXPECT_EQ(chunk.size(), 4);

    /// First MPEG-G Record with reference sequence chr1
//    auto rec = chunk.getData().begin();
    auto rec = chunk.begin();
    EXPECT_EQ(rec->getNumberOfTemplateSegments(), 2);
    EXPECT_EQ(rec->getSegments().size(), 2);
    EXPECT_EQ(rec->getAlignments().size(), 3);
    EXPECT_EQ(rec->getClassID(), genie::core::record::ClassType::CLASS_I);
    EXPECT_EQ(rec->getFlags(), 4);

    auto rec_alignment = rec->getAlignments().begin();
    EXPECT_EQ(rec_alignment->getPosition(), 10541-1);
    EXPECT_EQ(rec_alignment->getAlignment().getECigar(), "76=");
    EXPECT_EQ(rec_alignment->getAlignment().getMappingScores().front(), 118);

    ASSERT_EQ(rec_alignment->getAlignmentSplits().front().get()->getType(),
              genie::core::record::AlignmentSplit::Type::SAME_REC);

    auto rec_alignment_split = dynamic_cast<genie::core::record::alignment_split::SameRec *>(
        rec_alignment->getAlignmentSplits().front().get());
    EXPECT_EQ(rec_alignment_split->getDelta(), 181879-10541);
    EXPECT_EQ(rec_alignment_split->getAlignment().getECigar(), "76=");
    EXPECT_EQ(rec_alignment_split->getAlignment().getMappingScores().front(), 118);

    /// Third MPEG-G Record with reference sequence GL877875.1
//    rec += 3;
    rec = std::next(rec, 3);
    EXPECT_EQ(rec->getNumberOfTemplateSegments(), 2);
    EXPECT_EQ(rec->getSegments().size(), 2);
    EXPECT_EQ(rec->getAlignments().size(), 1);
    EXPECT_EQ(rec->getClassID(), genie::core::record::ClassType::CLASS_I);
    EXPECT_EQ(rec->getFlags(), 4);

    /// Expect first alignment comes from record with FLAG 419 due to swapping
    rec_alignment = rec->getAlignments().begin();
    EXPECT_EQ(rec_alignment->getPosition(), 658-1);
    EXPECT_EQ(rec_alignment->getAlignment().getECigar(), "76=");
    EXPECT_EQ(rec_alignment->getAlignment().getMappingScores().front(), 100);

    ASSERT_EQ(rec_alignment->getAlignmentSplits().front().get()->getType(),
              genie::core::record::AlignmentSplit::Type::SAME_REC);

    rec_alignment_split = dynamic_cast<genie::core::record::alignment_split::SameRec *>(
        rec_alignment->getAlignmentSplits().front().get());
    EXPECT_EQ(rec_alignment_split->getDelta(), 1491-658);
    EXPECT_EQ(rec_alignment_split->getAlignment().getECigar(), "76=");
    EXPECT_EQ(rec_alignment_split->getAlignment().getMappingScores().front(), 100);

}

TEST(SAM, ImporterPairReadsMultipleAlignmentsSplitRec) {  // NOLINT(cert-err-cpp)

    // Test sam file containing alignments with invalid flags
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::ifstream f(gitRootDir + "/data/sam/pair_reads_multi_alignments.sam");
    UTILS_DIE_IF(!f.good(), "Cannot read file");

    genie::format::sam::Reader reader(f);

    std::list<std::string> lines;
    std::list<genie::format::sam::ReadTemplate> rts;
    genie::format::sam::ReadTemplateGroup rtg;

//    genie::core::record::Chunk chunk;
    std::list<genie::core::record::Record> chunk;
    {
        while (reader.good()){
            reader.read(lines);
        }
    }

    rtg.addRecords(lines);
    lines.clear();
    rtg.getTemplates(rts);

    ASSERT_EQ(rts.size(), 1);

//    genie::format::sam::Importer::convert(chunk.getData(), rts.front(), reader.getRefs(), true);
    genie::format::sam::Importer::convert(chunk, rts.front(), reader.getRefs(), true);

//    EXPECT_EQ(chunk.getData().size(), 8);
    EXPECT_EQ(chunk.size(), 8);

    /// First read template reference sequence chr1
    /// Read 1
//    auto rec = chunk.getData().begin();
    auto rec = chunk.begin();
    {
        EXPECT_EQ(rec->getNumberOfTemplateSegments(), 2);
        EXPECT_EQ(rec->getSegments().size(), 1);
        EXPECT_EQ(rec->getAlignments().size(), 3);
        EXPECT_EQ(rec->getClassID(), genie::core::record::ClassType::CLASS_I);
        EXPECT_EQ(rec->getFlags(), 4);

        auto rec_alignment = rec->getAlignments().begin();
        EXPECT_EQ(rec_alignment->getPosition(), 181879-1);
        EXPECT_EQ(rec_alignment->getAlignment().getECigar(), "76=");
        EXPECT_EQ(rec_alignment->getAlignment().getMappingScores().front(), 118);

        auto rec_alignment_split = dynamic_cast<genie::core::record::alignment_split::OtherRec *>(
            rec_alignment->getAlignmentSplits().front().get());
        EXPECT_EQ(rec_alignment_split->getNextPos(), 10541-1);
        EXPECT_EQ(rec_alignment_split->getNextSeq(), 0);

        auto rec_more_alignment_info = dynamic_cast<const genie::core::record::alignment_external::OtherRec *>(
            &(rec->getAlignmentExternal()));
        EXPECT_EQ(rec_more_alignment_info->getNextPos(), 113602003-1);
        EXPECT_EQ(rec_more_alignment_info->getNextSeq(), 1);
    }

    /// Read 2
//    rec += 1;
    rec = std::next(rec);
    {
        EXPECT_EQ(rec->getNumberOfTemplateSegments(), 2);
        EXPECT_EQ(rec->getSegments().size(), 1);
        EXPECT_EQ(rec->getAlignments().size(), 3);
        EXPECT_EQ(rec->getClassID(), genie::core::record::ClassType::CLASS_I);
        EXPECT_EQ(rec->getFlags(), 4);

        auto rec_alignment = rec->getAlignments().begin();
        EXPECT_EQ(rec_alignment->getPosition(), 10541-1);
        EXPECT_EQ(rec_alignment->getAlignment().getECigar(), "76=");
        EXPECT_EQ(rec_alignment->getAlignment().getMappingScores().front(), 118);

        auto rec_alignment_split = dynamic_cast<genie::core::record::alignment_split::OtherRec *>(
            rec_alignment->getAlignmentSplits().front().get());
        EXPECT_EQ(rec_alignment_split->getNextPos(), 181879-1);
        EXPECT_EQ(rec_alignment_split->getNextSeq(), 0);

        auto rec_more_alignment_info = dynamic_cast<const genie::core::record::alignment_external::OtherRec *>(
            &(rec->getAlignmentExternal()));
        EXPECT_EQ(rec_more_alignment_info->getNextPos(), 113602258-1);
        EXPECT_EQ(rec_more_alignment_info->getNextSeq(), 1);
    }

    /// Third MPEG-G Record with reference sequence GL877875.1
    /// Read 1
//    rec += 5;
    rec = std::next(rec, 5);
    {
        EXPECT_EQ(rec->getNumberOfTemplateSegments(), 2);
        EXPECT_EQ(rec->getSegments().size(), 1);
        EXPECT_EQ(rec->getAlignments().size(), 1);
        EXPECT_EQ(rec->getClassID(), genie::core::record::ClassType::CLASS_I);
        EXPECT_EQ(rec->getFlags(), 4);

        auto rec_alignment = rec->getAlignments().begin();
        EXPECT_EQ(rec_alignment->getPosition(), 1491-1);
        EXPECT_EQ(rec_alignment->getAlignment().getECigar(), "76=");
        EXPECT_EQ(rec_alignment->getAlignment().getMappingScores().front(), 100);

        auto rec_alignment_split = dynamic_cast<genie::core::record::alignment_split::OtherRec *>(
            rec_alignment->getAlignmentSplits().front().get());
        EXPECT_EQ(rec_alignment_split->getNextPos(), 658-1);
        EXPECT_EQ(rec_alignment_split->getNextSeq(), 3);
    }

    /// Read 2
//    rec += 1;
    rec = std::next(rec);
    {
        EXPECT_EQ(rec->getNumberOfTemplateSegments(), 2);
        EXPECT_EQ(rec->getSegments().size(), 1);
        EXPECT_EQ(rec->getAlignments().size(), 1);
        EXPECT_EQ(rec->getClassID(), genie::core::record::ClassType::CLASS_I);
        EXPECT_EQ(rec->getFlags(), 4);

        auto rec_alignment = rec->getAlignments().begin();
        EXPECT_EQ(rec_alignment->getPosition(), 658-1);
        EXPECT_EQ(rec_alignment->getAlignment().getECigar(), "76=");
        EXPECT_EQ(rec_alignment->getAlignment().getMappingScores().front(), 100);

        auto rec_alignment_split = dynamic_cast<genie::core::record::alignment_split::OtherRec *>(
            rec_alignment->getAlignmentSplits().front().get());
        EXPECT_EQ(rec_alignment_split->getNextPos(), 1491-1);
        EXPECT_EQ(rec_alignment_split->getNextSeq(), 3);
    }

}

# if true
TEST(SamFileReader, ImportFromFile2) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

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
//    std::ifstream f("/home/adhisant/tmp/data/SAM_TEST/" + fnames[22]);

    std::ifstream f(gitRootDir + "/data/sam/thirteen-records.sam");
    UTILS_DIE_IF(!f.good(), "Cannot read file");

    genie::format::sam::Reader reader(f);

    std::list<std::string> lines;
    std::list<genie::format::sam::ReadTemplate> rts;
    genie::format::sam::ReadTemplateGroup rtg;

//    genie::core::record::Chunk chunk;
    std::list<genie::core::record::Record> records;
    int k = 0;
    {
        while (reader.good() && k < 1000){
            reader.read(lines);
            k++;
        }
    }

    rtg.addRecords(lines);
    lines.clear();
    rtg.getTemplates(rts);

    for (auto& rt: rts){
//        genie::format::sam::Importer::convert(chunk, rt, reader.getRefs(), false);
        genie::format::sam::Importer::convert(records, rt, reader.getRefs(), false);
    }

//    auto& records = chunk.getData();
//    std::sort (records.begin(), records.end(), genie::format::sam::Importer::compare);
    records.sort(genie::format::sam::Importer::compare);

    uint64_t last_position = 0;
    for (const auto &record : records) {
        if (record.getAlignments().empty()) {
            continue;
        }

        auto minPos = genie::format::sam::Importer::getMinPos(record);

        UTILS_DIE_IF(minPos < last_position, "Records not properly ordered");
        last_position = minPos;
    }

}
#endif

# if false
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
#endif




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

*/