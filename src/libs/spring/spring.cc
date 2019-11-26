#include "spring.h"
#include <gabac/gabac.h>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem@e980ed0/filesystem.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "call-template-functions.h"
#include "combine-aus.h"
#include "compress-ureads.h"
#include "encoder.h"
#include "generate-new-fastq.h"
#include "generate-read-streams.h"
#include "params.h"
#include "preprocess.h"
#include "reorder-compress-quality-id.h"
#include "reorder.h"
#include "spring-gabac.h"
#include "util.h"

namespace spring {

void generate_streams_SPRING(util::FastqFileReader *fastqFileReader1, util::FastqFileReader *fastqFileReader2,
                             int num_thr, bool paired_end, const std::string &working_dir, bool,
                             const std::string &outputFilePath, bool ureads_flag, bool preserve_quality,
                             bool preserve_id) {
#ifdef GENIE_USE_OPENMP
    std::cout << "SPRING: built with OpenMP" << std::endl;
#else
    std::cout << "SPRING: *not* built with OpenMP" << std::endl;
#endif

    // generate random temp directory in the working directory
    std::string temp_dir;
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
    auto compression_start = std::chrono::steady_clock::now();

    std::string infile_1, infile_2, outfile;
    compression_params *cp_ptr = new compression_params;
    memset(cp_ptr, 0, sizeof(compression_params));
    compression_params &cp = *cp_ptr;
    cp.paired_end = paired_end;
    cp.preserve_id = preserve_id;
    cp.preserve_quality = preserve_quality;
    cp.num_thr = num_thr;
    cp.ureads_flag = ureads_flag;
    cp.num_reads_per_block = cp.ureads_flag ? NUM_READS_PER_BLOCK_UREADS : NUM_READS_PER_BLOCK;

#if GENIE_USE_OPENMP
    //
    // Ensure that omp parallel regions are executed with the requested
    // #threads.
    //
    int omp_dyn_var = omp_get_dynamic();
    omp_set_dynamic(0);
#endif

    std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> descriptorFilesPerAUs;
    if (cp.ureads_flag) {
        std::cout << "ureads_flag detected.\n";
        // TODO: add support for analyze
        compress_ureads(fastqFileReader1, fastqFileReader2, temp_dir, cp, outputFilePath);
        //        descriptorFilesPerAUs = compress_ureads(fastqFileReader1,
        //        fastqFileReader2, temp_dir, cp, st); cp.num_blocks =
        //        descriptorFilesPerAUs.size();
    } else {
        std::cout << "Preprocessing ...\n";
        auto preprocess_start = std::chrono::steady_clock::now();
        preprocess(fastqFileReader1, fastqFileReader2, temp_dir, cp);
        auto preprocess_end = std::chrono::steady_clock::now();
        std::cout << "Preprocessing done!\n";
        std::cout << "Time for this step: "
                  << std::chrono::duration_cast<std::chrono::seconds>(preprocess_end - preprocess_start).count()
                  << " s\n";
//        if (stats->enabled) {
 //           stats->preprocess_t = preprocess_end - preprocess_start;
  //      }

        std::cout << "Reordering ...\n";
        auto reorder_start = std::chrono::steady_clock::now();
        call_reorder(temp_dir, cp);
        auto reorder_end = std::chrono::steady_clock::now();
        std::cout << "Reordering done!\n";
        std::cout << "Time for this step: "
                  << std::chrono::duration_cast<std::chrono::seconds>(reorder_end - reorder_start).count() << " s\n";
   //     if (stats->enabled) {
   //         stats->reorder_t = reorder_end - reorder_start;
   //     }

        std::cout << "Encoding ...\n";
        auto encoder_start = std::chrono::steady_clock::now();
        call_encoder(temp_dir, cp);
        auto encoder_end = std::chrono::steady_clock::now();
        std::cout << "Encoding done!\n";
        std::cout << "Time for this step: "
                  << std::chrono::duration_cast<std::chrono::seconds>(encoder_end - encoder_start).count() << " s\n";
    //    if (stats->enabled) {
    //        stats->encode_t = encoder_end - encoder_start;
    //    }

        std::vector<std::vector<gabac::EncodingConfiguration>> configs = create_default_conf();

        std::cout << "Generating read streams ...\n";
        auto grs_start = std::chrono::steady_clock::now();
        generate_read_streams(temp_dir, cp, configs);
        auto grs_end = std::chrono::steady_clock::now();
        std::cout << "Generating read streams done!\n";
        std::cout << "Time for this step: "
                  << std::chrono::duration_cast<std::chrono::seconds>(grs_end - grs_start).count() << " s\n";
#if 0
        // TODO: do this only when debugging, otherwise disable for speed 
        std::cout << "Generating new FASTQ for testing purposes\n";
        auto new_fq_start = std::chrono::steady_clock::now();
        if (!cp.paired_end) {
            std::cout << "File name: " << outputFilePath+".new.fastq\n";
            generate_new_fastq_se(fastqFileReader1, temp_dir, cp, outputFilePath);
        } else {
            std::cout << "File name: " << outputFilePath+".new_1.fastq\n";
            std::cout << "File name: " << outputFilePath+".new_2.fastq\n";
            generate_new_fastq_pe(fastqFileReader1, fastqFileReader2, temp_dir,
        cp, outputFilePath);
        }
        auto new_fq_end = std::chrono::steady_clock::now();
        std::cout << "Generating new FASTQ done (for testing)\n";
        std::cout << "Time for this step: "
                  << std::chrono::duration_cast<std::chrono::seconds>(new_fq_end -
                                                                      new_fq_start)
                         .count()
                  << " s\n";
#endif
        if (preserve_quality || preserve_id) {
            std::cout << "Reordering and compressing quality and/or ids ...\n";
            auto rcqi_start = std::chrono::steady_clock::now();
            reorder_compress_quality_id(temp_dir, cp, configs);
            auto rcqi_end = std::chrono::steady_clock::now();
            std::cout << "Reordering and compressing quality and/or ids done!\n";
            std::cout << "Time for this step: "
                      <<
                      std::chrono::duration_cast<std::chrono::seconds>(rcqi_end -
                      rcqi_start).count() << " s\n";
        }

        std::cout << "Combining AUs and writing compressed file ...\n";
        auto cau_start = std::chrono::steady_clock::now();
        combine_aus(temp_dir, cp, configs, outputFilePath);
        auto cau_end = std::chrono::steady_clock::now();
        std::cout << "Combining AUs done!\n";
        std::cout << "Time for this step: "
                  << std::chrono::duration_cast<std::chrono::seconds>(cau_end - cau_start).count() << " s\n";
    }

    // generated_aus result(descriptorFilesPerAUs);
    // // Write compression params to a file
    // std::string compression_params_file = "cp.bin";
    // gabac::DataBlock d((uint8_t *)&cp, sizeof(compression_params), 1);
    // st.compress(compression_params_file, &d);
    // st.pack(d, compression_params_file);

    delete cp_ptr;

    auto compression_end = std::chrono::steady_clock::now();
    std::cout << "Compression done!\n";
    std::cout << "Total time for compression: "
              << std::chrono::duration_cast<std::chrono::seconds>(compression_end - compression_start).count()
              << " s\n";

    ghc::filesystem::remove_all(temp_dir);

    //    st.finish();

#if GENIE_USE_OPENMP
    //
    // Restore the dyn-var omp variable.
    //
    omp_set_dynamic(omp_dyn_var);
#endif

    return;
}

std::string random_string(size_t length) {
    auto randchar = []() -> char {
        const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

}  // namespace spring
