#include <algorithm>
#include <filesystem/filesystem.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>  // std::setw
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "spring/encoder.h"
#include "spring/params.h"
#include "spring/preprocess.h"
#include "spring/reorder.h"
#include "spring/reorder-compress-quality-id.h"
#include "spring/generate-read-streams.h"
#include "spring/decompress.h"
#include "spring/generate-new-fastq.h"
#include "spring/spring.h"
#include "spring/util.h"
#include "gabac/gabac.h"
#include "utils/fastq-file-reader.h"

namespace spring {

    generated_aus generate_streams_SPRING(
            utils::FastqFileReader *fastqFileReader1,
            utils::FastqFileReader *fastqFileReader2, int num_thr,
            bool paired_end, const std::string &working_dir, bool analyze, dsg::StreamSaver &st) {
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
        bool preserve_quality = true, preserve_id = true;
        compression_params *cp_ptr = new compression_params;
        memset(cp_ptr, 0, sizeof(compression_params));
        compression_params &cp = *cp_ptr;
        cp.paired_end = paired_end;
        cp.preserve_id = preserve_id;
        cp.preserve_quality = preserve_quality;
        cp.num_reads_per_block = NUM_READS_PER_BLOCK;
        cp.num_thr = num_thr;

#if GENIE_USE_OPENMP
       //
       // Ensure that omp parallel regions are executed with the requested
       // #threads.
       //
       int omp_dyn_var = omp_get_dynamic();
       omp_set_dynamic(0);
#endif

        std::cout << "Preprocessing ...\n";
        auto preprocess_start = std::chrono::steady_clock::now();
        preprocess(fastqFileReader1, fastqFileReader2, temp_dir, cp);
        auto preprocess_end = std::chrono::steady_clock::now();
        std::cout << "Preprocessing done!\n";
        std::cout << "Time for this step: "
                  << std::chrono::duration_cast<std::chrono::seconds>(
                          preprocess_end - preprocess_start)
                          .count()
                  << " s\n";

        std::cout << "Reordering ...\n";
        auto reorder_start = std::chrono::steady_clock::now();
        call_reorder(temp_dir, cp);
        auto reorder_end = std::chrono::steady_clock::now();
        std::cout << "Reordering done!\n";
        std::cout << "Time for this step: "
                  << std::chrono::duration_cast<std::chrono::seconds>(reorder_end -
                                                                      reorder_start)
                          .count()
                  << " s\n";

        std::cout << "Encoding ...\n";
        auto encoder_start = std::chrono::steady_clock::now();
        call_encoder(temp_dir, cp);
        auto encoder_end = std::chrono::steady_clock::now();
        std::cout << "Encoding done!\n";
        std::cout << "Time for this step: "
                  << std::chrono::duration_cast<std::chrono::seconds>(encoder_end -
                                                                      encoder_start)
                          .count()
                  << " s\n";

        std::cout << "Generating read streams ...\n";
        auto grs_start = std::chrono::steady_clock::now();
        auto descriptorFilesPerAUs = generate_read_streams(temp_dir, cp, analyze, st);
        auto grs_end = std::chrono::steady_clock::now();
        std::cout << "Generating read streams done!\n";
        std::cout << "Time for this step: "
                  << std::chrono::duration_cast<std::chrono::seconds>(grs_end -
                                                                      grs_start)
                          .count()
                  << " s\n";

        cp.num_blocks = descriptorFilesPerAUs.size();

        /* std::cout << "Generating new FASTQ\n";
         auto new_fq_start = std::chrono::steady_clock::now();
         if (!cp.paired_end) {
             generate_new_fastq_se(fastqFileReader1, temp_dir, cp);
         } else {
             generate_new_fastq_pe(fastqFileReader1, fastqFileReader2, temp_dir, cp);
         }
         auto new_fq_end = std::chrono::steady_clock::now();
         std::cout << "Generating new FASTQ done (for testing)\n";
         std::cout << "Time for this step: "
                   << std::chrono::duration_cast<std::chrono::seconds>(new_fq_end -
                                                                       new_fq_start)
                          .count()
                   << " s\n";*/

        generated_aus result(descriptorFilesPerAUs);
        if (preserve_quality || preserve_id) {
            std::cout << "Reordering and compressing quality and/or ids ...\n";
            auto rcqi_start = std::chrono::steady_clock::now();
            reorder_compress_quality_id(temp_dir, cp, analyze, st);
            auto rcqi_end = std::chrono::steady_clock::now();
            std::cout << "Reordering and compressing quality and/or ids done!\n";
            std::cout << "Time for this step: "
                      << std::chrono::duration_cast<std::chrono::seconds>(rcqi_end -
                                                                          rcqi_start)
                              .count()
                      << " s\n";
        }

        // Write compression params to a file
        std::string compression_params_file = "cp.bin";
        gabac::DataBlock d((uint8_t *) &cp, sizeof(compression_params), 1);
        st.compress(compression_params_file, &d);
        st.pack(d, compression_params_file);

        delete cp_ptr;

        // TODO: remove temporary files
        remove((temp_dir + "/read_order.bin").c_str());

        auto compression_end = std::chrono::steady_clock::now();
        std::cout << "Compression done!\n";
        std::cout << "Total time for compression: "
                  << std::chrono::duration_cast<std::chrono::seconds>(
                          compression_end - compression_start)
                          .count()
                  << " s\n";

        /*// test decompression
        auto decompression_start = std::chrono::steady_clock::now();
        decompress(temp_dir);
        auto decompression_end = std::chrono::steady_clock::now();
        std::cout << "Decompression done!\n";
        std::cout << "Total time for decompression: "
                  << std::chrono::duration_cast<std::chrono::seconds>(
                         decompression_end - decompression_start)
                         .count()
                  << " s\n";*/

        ghc::filesystem::remove_all(temp_dir);

        st.finish();

#if GENIE_USE_OPENMP
        //
        // Restore the dyn-var omp variable.
        //
        omp_set_dynamic(omp_dyn_var);
#endif

        return result;
    }

    void call_reorder(const std::string &temp_dir, compression_params &cp) {
        size_t bitset_size_reorder = (2 * cp.max_readlen - 1) / 64 * 64 + 64;
        switch (bitset_size_reorder) {
            case 64:
                reorder_main<64>(temp_dir, cp);
                break;
            case 128:
                reorder_main<128>(temp_dir, cp);
                break;
            case 192:
                reorder_main<192>(temp_dir, cp);
                break;
            case 256:
                reorder_main<256>(temp_dir, cp);
                break;
            case 320:
                reorder_main<320>(temp_dir, cp);
                break;
            case 384:
                reorder_main<384>(temp_dir, cp);
                break;
            case 448:
                reorder_main<448>(temp_dir, cp);
                break;
            case 512:
                reorder_main<512>(temp_dir, cp);
                break;
            case 576:
                reorder_main<576>(temp_dir, cp);
                break;
            case 640:
                reorder_main<640>(temp_dir, cp);
                break;
            case 704:
                reorder_main<704>(temp_dir, cp);
                break;
            case 768:
                reorder_main<768>(temp_dir, cp);
                break;
            case 832:
                reorder_main<832>(temp_dir, cp);
                break;
            case 896:
                reorder_main<896>(temp_dir, cp);
                break;
            case 960:
                reorder_main<960>(temp_dir, cp);
                break;
            case 1024:
                reorder_main<1024>(temp_dir, cp);
                break;
            default:
                throw std::runtime_error("Wrong bitset size.");
        }
    }

    void call_encoder(const std::string &temp_dir, compression_params &cp) {
        size_t bitset_size_encoder = (3 * cp.max_readlen - 1) / 64 * 64 + 64;
        switch (bitset_size_encoder) {
            case 64:
                encoder_main<64>(temp_dir, cp);
                break;
            case 128:
                encoder_main<128>(temp_dir, cp);
                break;
            case 192:
                encoder_main<192>(temp_dir, cp);
                break;
            case 256:
                encoder_main<256>(temp_dir, cp);
                break;
            case 320:
                encoder_main<320>(temp_dir, cp);
                break;
            case 384:
                encoder_main<384>(temp_dir, cp);
                break;
            case 448:
                encoder_main<448>(temp_dir, cp);
                break;
            case 512:
                encoder_main<512>(temp_dir, cp);
                break;
            case 576:
                encoder_main<576>(temp_dir, cp);
                break;
            case 640:
                encoder_main<640>(temp_dir, cp);
                break;
            case 704:
                encoder_main<704>(temp_dir, cp);
                break;
            case 768:
                encoder_main<768>(temp_dir, cp);
                break;
            case 832:
                encoder_main<832>(temp_dir, cp);
                break;
            case 896:
                encoder_main<896>(temp_dir, cp);
                break;
            case 960:
                encoder_main<960>(temp_dir, cp);
                break;
            case 1024:
                encoder_main<1024>(temp_dir, cp);
                break;
            case 1088:
                encoder_main<1088>(temp_dir, cp);
                break;
            case 1152:
                encoder_main<1152>(temp_dir, cp);
                break;
            case 1216:
                encoder_main<1216>(temp_dir, cp);
                break;
            case 1280:
                encoder_main<1280>(temp_dir, cp);
                break;
            case 1344:
                encoder_main<1344>(temp_dir, cp);
                break;
            case 1408:
                encoder_main<1408>(temp_dir, cp);
                break;
            case 1472:
                encoder_main<1472>(temp_dir, cp);
                break;
            case 1536:
                encoder_main<1536>(temp_dir, cp);
                break;
            default:
                throw std::runtime_error("Wrong bitset size.");
        }
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
