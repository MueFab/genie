#include <string>
#include "input/fastq/FastqFileReader.h"

uint8_t find_id_pattern(std::string &id_1, std::string &id_2);

bool check_id_pattern(std::string &id_1, std::string &id_2,
                      uint8_t paired_id_code);

int preprocess(dsg::input::fastq::FastqFileReader &fastqFileReader1, dsg::input::fastq::FastqFileReader &fastqFileReader2, std::string &working_dir, bool paired_end, bool preserve_id, bool preserve_quality);
