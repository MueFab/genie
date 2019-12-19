#include "spring-encoder.h"
#include <spring/params.h>
#include <spring/spring.h>
#include <chrono>

#include <format/mpegg_rec/alignment-container.h>
#include <format/mpegg_rec/external-alignment.h>
#include <format/mpegg_rec/meta-alignment.h>
#include <format/mpegg_rec/mpegg-record.h>
#include <format/mpegg_rec/segment.h>
#include <spring/generate-read-streams.h>

void SpringEncoder::preprocessInit() {
    std::string outfileclean[2];
    std::string outfileid;
    std::string outfilequality[2];
    std::string outfilereadlength[2];
    std::string basedir = temp_dir;
    outfileclean[0] = basedir + "/input_clean_1.dna";
    outfileclean[1] = basedir + "/input_clean_2.dna";
    outfileN[0] = basedir + "/input_N.dna";
    outfileN[1] = basedir + "/input_N.dna.2";
    outfileorderN[0] = basedir + "/read_order_N.bin";
    outfileorderN[1] = basedir + "/read_order_N.bin.2";
    outfileid = basedir + "/id_1";
    outfilequality[0] = basedir + "/quality_1";
    outfilequality[1] = basedir + "/quality_2";

    for (int j = 0; j < 2; j++) {
        if (j == 1 && !cp.paired_end) continue;
        fout_clean[j].open(outfileclean[j]);
        fout_N[j].open(outfileN[j]);
        fout_order_N[j].open(outfileorderN[j], std::ios::binary);
        if (cp.preserve_quality) fout_quality[j].open(outfilequality[j]);
    }
    if (cp.preserve_id) fout_id.open(outfileid);

    max_readlen = 0;
    num_reads[0] = 0;
    num_reads[1] = 0;
    num_reads_clean[0] = 0;
    num_reads_clean[1] = 0;
    uint32_t num_reads_per_block;
    num_reads_per_block = cp.num_reads_per_block;

    num_threads = 1;

    uint64_t num_reads_per_step = (uint64_t)num_threads * num_reads_per_block;
    read_contains_N_array = new bool[num_reads_per_step];
    read_lengths_array = new uint32_t[num_reads_per_step];
    num_blocks_done = 0;
}

void SpringEncoder::preprocessIteration(std::unique_ptr<format::mpegg_rec::MpeggChunk> t) {
    for (int j = 0; j < 2; j++) {
        if (j == 1 && !cp.paired_end) continue;
        if (num_reads[0] + num_reads[1] + (cp.paired_end ? t->size() * 2 : t->size()) > spring::MAX_NUM_READS) {
            std::cerr << "Max number of reads allowed is " << spring::MAX_NUM_READS << "\n";
            throw std::runtime_error("Too many reads.");
        }
        {
            // check if reads and qualities have equal lengths
            for (uint32_t i = 0; i < t->size(); i++) {
                size_t len = (*t)[i]->getRecordSegment(j)->getSequence()->length();
                if (len == 0) throw std::runtime_error("Read of length 0 detected.");
                if (len > spring::MAX_READ_LEN) {
                    std::cerr << "Max read length without ureads mode is " << spring::MAX_READ_LEN
                              << ", but found read of length " << len << "\n";
                    throw std::runtime_error("Too long read length");  // (please try --long/-l flag).");
                }
                if (cp.preserve_quality && ((*t)[i]->getRecordSegment(j)->getQuality(0)->length() != len))
                    throw std::runtime_error("Read length does not match quality length.");
                if (cp.preserve_id && ((*t)[i]->getReadName().length() == 0) && j == 0)
                    throw std::runtime_error("Identifier of length 0 detected.");
                read_lengths_array[i] = (uint32_t)len;

                read_contains_N_array[i] =
                    ((*t)[i]->getRecordSegment(j)->getSequence()->find('N') != std::string::npos);
            }
        }  // omp parallel
        // write reads and read_order_N to respective files
        for (uint32_t i = 0; i < t->size(); i++) {
            if (!read_contains_N_array[i]) {
                fout_clean[j] << *(*t)[i]->getRecordSegment(j)->getSequence() << "\n";
                num_reads_clean[j]++;
            } else {
                uint32_t pos_N = num_reads[j] + i;
                fout_order_N[j].write((char *)&pos_N, sizeof(uint32_t));
                fout_N[j] << *(*t)[i]->getRecordSegment(j)->getSequence() << "\n";
            }
        }
        //          // write qualities to file
        for (uint32_t i = 0; i < t->size(); i++)
            fout_quality[j] << *(*t)[i]->getRecordSegment(j)->getQuality(0) << "\n";
        // write ids to file
        if (j == 0)
            for (uint32_t i = 0; i < t->size(); i++) fout_id << (*t)[i]->getReadName() << "\n";
        num_reads[j] += t->size();
        max_readlen = std::max(max_readlen, *(std::max_element(read_lengths_array, read_lengths_array + t->size())));
    }
    if (cp.paired_end)
        if (num_reads[0] != num_reads[1]) throw std::runtime_error("Number of reads in paired files do not match.");
    num_blocks_done += num_threads;
}

void SpringEncoder::preprocessClean() {
    delete[] read_contains_N_array;
    delete[] read_lengths_array;
    //  delete[] quality_binning_table;
    // close files
    for (int j = 0; j < 2; j++) {
        if (j == 1 && !cp.paired_end) continue;
        fout_clean[j].close();
        fout_N[j].close();
        fout_order_N[j].close();
        if (cp.preserve_quality) fout_quality[j].close();
    }
    if (cp.preserve_id) fout_id.close();

    if (num_reads[0] == 0) throw std::runtime_error("No reads found.");

    if (cp.paired_end) {
        // merge input_N and input_order_N for the two files
        std::ofstream fout_N_PE(outfileN[0], std::ios::app);
        std::ifstream fin_N_PE(outfileN[1]);
        fout_N_PE << fin_N_PE.rdbuf();
        fout_N_PE.close();
        fin_N_PE.close();
        remove(outfileN[1].c_str());
        std::ofstream fout_order_N_PE(outfileorderN[0], std::ios::app | std::ios::binary);
        std::ifstream fin_order_N(outfileorderN[1], std::ios::binary);
        uint32_t num_N_file_2 = num_reads[1] - num_reads_clean[1];
        uint32_t order_N;
        for (uint32_t i = 0; i < num_N_file_2; i++) {
            fin_order_N.read((char *)&order_N, sizeof(uint32_t));
            order_N += num_reads[0];
            fout_order_N_PE.write((char *)&order_N, sizeof(uint32_t));
        }
        fin_order_N.close();
        fout_order_N_PE.close();
        remove(outfileorderN[1].c_str());
    }

    cp.num_reads = num_reads[0] + num_reads[1];
    cp.num_reads_clean[0] = num_reads_clean[0];
    cp.num_reads_clean[1] = num_reads_clean[1];
    cp.max_readlen = max_readlen;

    std::cout << "Max Read length: " << cp.max_readlen << "\n";
    std::cout << "Total number of reads: " << cp.num_reads << "\n";

    std::cout << "Total number of reads without N: " << cp.num_reads_clean[0] + cp.num_reads_clean[1] << "\n";
}

SpringEncoder::SpringEncoder(int _num_thr, std::string _working_dir, bool _ureads_flag, bool _preserve_quality,
                             bool _preserve_id, util::FastqStats *_stats)
    : num_thr(_num_thr),
      working_dir(std::move(_working_dir)),
      preserve_quality(_preserve_quality),
      preserve_id(_preserve_id) {
    // generate random temp directory in the working directory
    while (true) {
        std::string random_str = "tmp." + spring::random_string(10);
        temp_dir = working_dir + "/" + random_str + '/';
        if (!ghc::filesystem::exists(temp_dir)) break;
    }
    if (!ghc::filesystem::create_directory(temp_dir)) {
        throw std::runtime_error("Cannot create temporary directory.");
    }
    std::cout << "Temporary directory: " << temp_dir << "\n";

    std::cout << "Starting compression...\n";
    compression_start = std::chrono::steady_clock::now();

    std::string infile_1, infile_2, outfile;
    memset(&cp, 0, sizeof(spring::compression_params));
    cp.preserve_id = preserve_id;
    cp.preserve_quality = preserve_quality;
    cp.num_thr = num_thr;
    cp.ureads_flag = _ureads_flag;
    cp.num_reads_per_block = cp.ureads_flag ? spring::NUM_READS_PER_BLOCK_UREADS : spring::NUM_READS_PER_BLOCK;

    std::cout << "Preprocessing ...\n";
    preprocess_start = std::chrono::steady_clock::now();
    preprocessInit();
}

void SpringEncoder::flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> t, size_t id) {
    if (!id) {
        cp.paired_end = (*t)[0]->getNumberOfRecords() > 1;
    }
    preprocessIteration(std::move(t));
}

void SpringEncoder::dryIn() {
    preprocessClean();
    auto preprocess_end = std::chrono::steady_clock::now();
    std::cout << "Preprocessing done!\n";
    std::cout << "Time for this step: "
              << std::chrono::duration_cast<std::chrono::seconds>(preprocess_end - preprocess_start).count() << " s\n";
    if (stats->enabled) {
        stats->preprocess_t = preprocess_end - preprocess_start;
    }

    std::cout << "Reordering ...\n";
    auto reorder_start = std::chrono::steady_clock::now();
    call_reorder(temp_dir, cp);
    auto reorder_end = std::chrono::steady_clock::now();
    std::cout << "Reordering done!\n";
    std::cout << "Time for this step: "
              << std::chrono::duration_cast<std::chrono::seconds>(reorder_end - reorder_start).count() << " s\n";
    if (stats->enabled) {
        stats->reorder_t = reorder_end - reorder_start;
    }

    std::cout << "Encoding ...\n";
    auto encoder_start = std::chrono::steady_clock::now();
    call_encoder(temp_dir, cp);
    auto encoder_end = std::chrono::steady_clock::now();
    std::cout << "Encoding done!\n";
    std::cout << "Time for this step: "
              << std::chrono::duration_cast<std::chrono::seconds>(encoder_end - encoder_start).count() << " s\n";
    if (stats->enabled) {
        stats->encode_t = encoder_end - encoder_start;
    }

    // --------------------------------------------------------------------------------------------------

    std::cout << "Generating read streams ...\n";
    auto grs_start = std::chrono::steady_clock::now();

    if (!cp.paired_end) {
        spring::se_data data;
        spring::loadSE_Data(cp, temp_dir, &data);
        for (size_t i = 0; i < spring::getNumBlocks(cp); ++i) {
            flowOut(spring::generate_subseqs(data, i), i);
        }
    } else {
        spring::se_data data;
        spring::pe_statistics statistics;
        spring::pe_block_data block_data;
        spring::loadPE_Data(cp, temp_dir, &data);
        generateBlocksPE(data, &block_data);
        data.order_arr.clear();
        for (size_t i = 0; i < spring::getNumBlocks(cp); ++i) {
            flowOut(spring::generate_streams_pe(data, block_data, i, &statistics), i);
        }
    }
    auto grs_end = std::chrono::steady_clock::now();
    std::cout << "Generating read streams done!\n";
    std::cout << "Time for this step: " << std::chrono::duration_cast<std::chrono::seconds>(grs_end - grs_start).count()
              << " s\n";
    if (stats->enabled) {
        stats->generation_t = grs_end - grs_start;
    }

    auto compression_end = std::chrono::steady_clock::now();
    std::cout << "Compression done!\n";
    std::cout << "Total time for compression: "
              << std::chrono::duration_cast<std::chrono::seconds>(compression_end - compression_start).count()
              << " s\n";

    ghc::filesystem::remove_all(temp_dir);

#if GENIE_USE_OPENMP
    //
    // Restore the dyn-var omp variable.
    //
    omp_set_dynamic(omp_dyn_var);
#endif

    dryOut();
}