#include "generate-new-fastq.h"
#include "util.h"

namespace spring {

void generate_new_fastq_se(util::FastqFileReader *fastqFileReader1, const std::string &temp_dir,
                           const compression_params &cp, const std::string &outputFilePath) {
    uint32_t numreads = cp.num_reads;
    std::string basedir = temp_dir;
    std::string file_order = basedir + "/read_order.bin";
    std::string outfile_fastq = outputFilePath + ".new.fastq";
    uint32_t *order_array;
    // array containing index mapping position in original fastq to
    // position after reordering
    order_array = new uint32_t[numreads];
    generate_order_array(file_order, order_array, numreads);

    // verify that order_array is a permutation of 1...numreads
    if (!is_permutation(order_array, numreads)) throw std::runtime_error("order_array not permutation of 1...numreads");

    uint32_t str_array_size = numreads / 10 + 1;
    // numreads/10+1 chosen so that these many FASTQ records can be stored in
    // memory without exceeding the RAM consumption of reordering stage
    std::string *read_array = new std::string[str_array_size];
    std::string *id_array = new std::string[str_array_size];
    std::string *quality_array = new std::string[str_array_size];

    std::ofstream fout_fastq(outfile_fastq);

    for (uint32_t i = 0; i <= numreads / str_array_size; i++) {
        uint32_t num_reads_bin = str_array_size;
        if (i == numreads / str_array_size) num_reads_bin = numreads % str_array_size;
        if (num_reads_bin == 0) break;
        uint32_t start_read_bin = i * str_array_size;
        uint32_t end_read_bin = i * str_array_size + num_reads_bin;
        // Read the file and pick up lines corresponding to this bin
        fastqFileReader1->seekFromSet(0);
        std::vector<util::FastqRecord> fastqRecords;
        for (uint32_t j = 0; j < numreads; j++) {
            fastqFileReader1->readRecords(1, &fastqRecords);
            if (order_array[j] >= start_read_bin && order_array[j] < end_read_bin) {
                uint32_t index = order_array[j] - start_read_bin;
                read_array[index] = fastqRecords[0].sequence;
                id_array[index] = fastqRecords[0].title;
                quality_array[index] = fastqRecords[0].qualityScores;
            }
        }
        for (uint32_t j = 0; j < num_reads_bin; j++) {
            fout_fastq << id_array[j] << "\n";
            fout_fastq << read_array[j] << "\n";
            fout_fastq << "+\n";
            fout_fastq << quality_array[j] << "\n";
        }
    }
    fout_fastq.close();
    delete[] order_array;
    delete[] id_array;
    delete[] read_array;
    delete[] quality_array;
    return;
}

void generate_new_fastq_pe(util::FastqFileReader *fastqFileReader1, util::FastqFileReader *fastqFileReader2,
                           const std::string &temp_dir, const compression_params &cp, const std::string &outputFilePath) {
    util::FastqFileReader *fastqFileReader[2] = {fastqFileReader1, fastqFileReader2};
    uint32_t numreads = cp.num_reads;
    std::string basedir = temp_dir;
    std::string file_order = basedir + "/order_quality.bin";
    std::string outfile_fastq_1 = outputFilePath + ".new_1.fastq";
    std::string outfile_fastq_2 = outputFilePath + ".new_2.fastq";
    uint32_t *order_array;
    // array containing index mapping position in original fastq to
    // position after reordering
    order_array = new uint32_t[numreads];
    generate_order_array(file_order, order_array, numreads);
    // verify that order_array is a permutation of 1...numreads
    if (!is_permutation(order_array, numreads)) throw std::runtime_error("order_array not permutation of 1...numreads");
    uint32_t str_array_size = numreads / 10 + 1;
    // numreads/10+1 chosen so that these many FASTQ records can be stored in
    // memory without exceeding the RAM consumption of reordering stage
    std::string *read_array = new std::string[str_array_size];
    std::string *id_array = new std::string[str_array_size];
    std::string *quality_array = new std::string[str_array_size];
    bool *first_file_flag_array = new bool[str_array_size];

    std::ofstream fout_fastq_1(outfile_fastq_1);
    std::ofstream fout_fastq_2(outfile_fastq_2);

    for (uint32_t i = 0; i <= numreads / str_array_size; i++) {
        uint32_t num_reads_bin = str_array_size;
        if (i == numreads / str_array_size) num_reads_bin = numreads % str_array_size;
        if (num_reads_bin == 0) break;
        uint32_t start_read_bin = i * str_array_size;
        uint32_t end_read_bin = i * str_array_size + num_reads_bin;
        // Read the file and pick up lines corresponding to this bin
        for (int k = 0; k < 2; k++) {
            fastqFileReader[k]->seekFromSet(0);
            std::vector<util::FastqRecord> fastqRecords;
            for (uint32_t j = k * numreads / 2; j < (k + 1) * numreads / 2; j++) {
                fastqFileReader[k]->readRecords(1, &fastqRecords);
                if (order_array[j] >= start_read_bin && order_array[j] < end_read_bin) {
                    uint32_t index = order_array[j] - start_read_bin;
                    read_array[index] = fastqRecords[0].sequence;
                    quality_array[index] = fastqRecords[0].qualityScores;
                }
                // ids treated in a different way since we always store ids from file 1
                if (k == 0) {
                    if (order_array[j] >= start_read_bin && order_array[j] < end_read_bin) {
                        uint32_t index = order_array[j] - start_read_bin;
                        first_file_flag_array[index] = true;
                        id_array[index] = fastqRecords[0].title;
                    }
                    if (order_array[numreads / 2 + j] >= start_read_bin &&
                        order_array[numreads / 2 + j] < end_read_bin) {
                        uint32_t index = order_array[numreads / 2 + j] - start_read_bin;
                        first_file_flag_array[index] = false;
                        id_array[index] = fastqRecords[0].title;
                    }
                }
            }
        }
        for (uint32_t j = 0; j < num_reads_bin; j++) {
            if (first_file_flag_array[j]) {
                fout_fastq_1 << id_array[j] << "\n";
                fout_fastq_1 << read_array[j] << "\n";
                fout_fastq_1 << "+\n";
                fout_fastq_1 << quality_array[j] << "\n";
            } else {
                fout_fastq_2 << id_array[j] << "\n";
                fout_fastq_2 << read_array[j] << "\n";
                fout_fastq_2 << "+\n";
                fout_fastq_2 << quality_array[j] << "\n";
            }
        }
    }
    fout_fastq_1.close();
    fout_fastq_2.close();
    delete[] order_array;
    delete[] id_array;
    delete[] read_array;
    delete[] quality_array;
    delete[] first_file_flag_array;
    return;
}

}  // namespace spring
