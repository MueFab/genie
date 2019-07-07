#ifndef SPRING_UTIL_H_
#define SPRING_UTIL_H_

#include <fstream>
#include <string>
#include <vector>

namespace spring {

    static const char chartorevchar[128] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'T',
            0, 'G', 0, 0, 0, 'C', 0, 0, 0, 0, 0, 0, 'N', 0, 0, 0, 0, 0, 'A', 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    struct compression_params {
        bool paired_end;
        bool preserve_quality;
        bool preserve_id;
//  bool long_flag;
//  bool qvz_flag;
//  bool ill_bin_flag;
//  bool bin_thr_flag;
//  double qvz_ratio;
//  unsigned int bin_thr_thr;
//  unsigned int bin_thr_high;
//  unsigned int bin_thr_low;
        uint32_t num_reads;
        uint32_t num_reads_clean[2];
        uint32_t max_readlen;
        uint8_t paired_id_code;
        bool paired_id_match;
        int num_reads_per_block;
//  int num_reads_per_block_long;
        int num_thr;
        uint32_t num_blocks;
    };

    uint32_t read_fastq_block(std::ifstream &fin, std::string *id_array,
                              std::string *read_array, std::string *quality_array,
                              const uint32_t &num_reads);

    void write_fastq_block(std::ofstream &fout, std::string *id_array,
                           std::string *read_array, std::string *quality_array,
                           const uint32_t &num_reads, const bool preserve_quality);

    void write_vector_to_file(const std::vector<int64_t> &subseq, const std::string &file_name);

    std::vector<int64_t> read_vector_from_file(const std::string &file_name);

    void quantize_quality(std::string *quality_array, const uint32_t &num_lines,
                          char *quantization_table);

    void generate_illumina_binning_table(char *illumina_binning_table);

    void generate_binary_binning_table(char *binary_binning_table, const unsigned int thr, const unsigned int high,
                                       const unsigned int low);

    uint8_t find_id_pattern(const std::string &id_1, const std::string &id_2);

    bool check_id_pattern(const std::string &id_1, const std::string &id_2,
                          const uint8_t paired_id_code);

    void modify_id(std::string &id, const uint8_t paired_id_code);

    void write_dna_in_bits(const std::string &read, std::ofstream &fout);

    void read_dna_from_bits(std::string &read, std::ifstream &fin);

    void reverse_complement(char *s, char *s1, const int readlen);

    std::string reverse_complement(const std::string &s, const int readlen);

    std::string read_file_as_string(const std::string &file_seq);

    void generate_order_array(const std::string &file_order, uint32_t *order_array,
                              const uint32_t &numreads);

    bool is_permutation(uint32_t *order_array, const uint32_t &numreads);

}  // namespace spring

#endif  // SPRING_UTIL_H_
