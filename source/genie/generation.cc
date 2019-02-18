// Copyright 2018 The genie authors


/**
 *  @file generation.c
 *  @brief Main entry point for descriptor stream generation algorithms
 *  @author Jan Voges
 *  @bug No known bugs
 */


#include "genie/generation.h"

#include <iostream>
#include <string>
#include <vector>
#include <utils/MPEGGFileCreation/MPEGGFileCreator.h>
#include <fileio/gabac_file.h>
#include <boost/filesystem.hpp>

#include "genie/exceptions.h"
#include "fileio/fasta_file_reader.h"
#include "fileio/fasta_record.h"
#include "fileio/fastq_file_reader.h"
#include "fileio/fastq_record.h"
#include "fileio/sam_file_reader.h"
#include "fileio/sam_record.h"
#include "coding/spring/spring.h"
#include "gabac/gabac.h"

namespace spring {
void decompress(const std::string& temp_dir,
                const std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>>& ref_descriptorFilesPerAU,
                uint32_t num_blocks,
                bool eru_abs_flag,
                bool paired_end
);
}

namespace dsg {


static void generationFromFasta(
        const ProgramOptions& programOptions
){
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "Descriptor stream generation from FASTA file" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    // Initialize a FASTA file reader.
    input::fasta::FastaFileReader fastaFileReader(programOptions.inputFilePath);

    // Parse the entire file.
    std::vector<input::fasta::FastaRecord> fastaRecords;
    fastaFileReader.parse(&fastaRecords);

    // Print information about the FASTA file.
    std::cout << "FASTA file: " << programOptions.inputFilePath << std::endl;
    std::cout << "Records: " << fastaRecords.size() << std::endl;

    // Iterate through all FASTA records.
    size_t i = 0;
    for (const auto& fastaRecord : fastaRecords) {
        std::cout << "  " << i++ << ": ";
        std::cout << fastaRecord.header << "\t";
        std::cout << fastaRecord.sequence << std::endl;
    }

    // Read FASTA lines.
    std::cout << "Lines: " << std::endl;
    while (true) {
        std::string line("");
        fastaFileReader.readLine(&line);

        if (line.empty() == true) {
            break;
        }

        std::cout << line << std::endl;
    }
}


static void generationFromFastq(
        const ProgramOptions& programOptions
){
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "Descriptor stream generation from FASTQ file" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    // Initialize a FASTQ file reader.
    input::fastq::FastqFileReader fastqFileReader(programOptions.inputFilePath);

    // Read FASTQ records in blocks of 10 records.
    size_t blockSize = 10;

    while (true) {
        std::vector<input::fastq::FastqRecord> fastqRecords;
        size_t numRecords = fastqFileReader.readRecords(blockSize, &fastqRecords);
        if (numRecords != blockSize) {
            std::cout << "Read only " << numRecords << " records (" << blockSize << " requested)." << std::endl;
        }

        // Iterate through the records.
        for (const auto& fastqRecord : fastqRecords) {
            std::cout << fastqRecord.title << "\t";
            std::cout << fastqRecord.sequence << "\t";
            std::cout << fastqRecord.optional << "\t";
            std::cout << fastqRecord.qualityScores << std::endl;
        }

        if (numRecords != blockSize) {
            break;
        }
    }
    // if (!programOptions.inputFilePairPath.empty()) {
    //     std::cout << "Paired file:\n";
    //     // Initialize a FASTQ file reader.
    //     input::fastq::FastqFileReader fastqFileReader1(programOptions.inputFilePairPath);
    //
    //     // Read FASTQ records in blocks of 10 records.
    //     size_t blockSize = 10;
    //
    //     while (true) {
    //         std::vector<input::fastq::FastqRecord> fastqRecords;
    //         size_t numRecords = fastqFileReader1.readRecords(blockSize, &fastqRecords);
    //         if (numRecords != blockSize) {
    //             std::cout << "Read only " << numRecords << " records (" << blockSize << " requested)." << std::endl;
    //         }
    //
    //         // Iterate through the records.
    //         for (const auto& fastqRecord : fastqRecords) {
    //             std::cout << fastqRecord.title << "\t";
    //             std::cout << fastqRecord.sequence << "\t";
    //             std::cout << fastqRecord.optional << "\t";
    //             std::cout << fastqRecord.qualityScores << std::endl;
    //         }
    //
    //         if (numRecords != blockSize) {
    //             break;
    //         }
    //     }
    // }
}


static generated_aus generationFromFastq_SPRING(
        const ProgramOptions& programOptions
){
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "Descriptor stream generation from FASTQ file" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    bool paired_end = false;
    // Initialize a FASTQ file reader.
    input::fastq::FastqFileReader fastqFileReader1(programOptions.inputFilePath);
    std::cout << "Calling SPRING" << std::endl;
    if (programOptions.inputFilePairPath.empty()) {
        return spring::generate_streams_SPRING(
                &fastqFileReader1,
                &fastqFileReader1,
                programOptions.numThreads,
                paired_end,
                programOptions.workingDirectory
        );
    } else {
        paired_end = true;
        input::fastq::FastqFileReader fastqFileReader2(programOptions.inputFilePairPath);
        return spring::generate_streams_SPRING(
                &fastqFileReader1,
                &fastqFileReader2,
                programOptions.numThreads,
                paired_end,
                programOptions.workingDirectory
        );
    }
}


static void generationFromSam(
        const ProgramOptions& programOptions
){
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "Descriptor stream generation from SAM file" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    // Initialize a SAM file reader.
    input::sam::SamFileReader samFileReader(programOptions.inputFilePath);

    // Get SAM header.
    std::cout << "-- SAM header begin" << std::endl;
    std::cout << samFileReader.header;
    std::cout << "-- SAM header end" << std::endl;

    // Read SAM records in blocks of 10 records.
    size_t blockSize = 10;

    while (true) {
        std::vector<input::sam::SamRecord> samRecords;
        size_t numRecords = samFileReader.readRecords(blockSize, &samRecords);
        if (numRecords != blockSize) {
            std::cout << "Read only " << numRecords << " records (" << blockSize << " requested)." << std::endl;
        }

        // Iterate through the records.
        for (const auto& samRecord : samRecords) {
            std::cout << samRecord.qname << "\t";
            std::cout << samRecord.rname << "\t";
            std::cout << samRecord.pos << "\t";
            std::cout << samRecord.cigar << "\t";
            std::cout << samRecord.seq << "\t";
            std::cout << samRecord.qual << std::endl;
        }

        if (numRecords != blockSize) {
            break;
        }
    }
}

std::string defaultGabacConf = "{\"word_size\":\"1\",\"sequence_transformation_id\":\"0\",\""
                               "sequence_transformation_parameter\":\"0\",\"transformed_sequences\""
                               ":[{\"lut_transformation_enabled\":\"0\",\"lut_transformation_bits\""
                               ":\"0\",\"lut_transformation_order\":\"0\",\"diff_coding_enabled\":\""
                               "0\",\"binarization_id\":\"0\",\"binarization_parameters\":[\"8\"],\""
                               "context_selection_id\":\"2\"}]}";


static void run_gabac(const std::vector<std::string>& files, bool decompress){
    for (const auto& file : files) {

        std::string config;
        std::string streamName = file.substr(file.find_last_of('/') + 1, std::string::npos);

        if (streamName.substr(0, 3) == "id.") {
            streamName = "id" + streamName.substr(streamName.find_first_of('.', 3), std::string::npos);
        } else {
            streamName = streamName.substr(0, streamName.find_last_of('.'));
        }

        std::string configpath = "../gabac_config/" +
                                 streamName +
                                 ".json";

        try {
            std::ifstream t(configpath);
            if (!t) {
                throw std::runtime_error("Config not existent");
            }
            config = std::string((std::istreambuf_iterator<char>(t)),
                                 std::istreambuf_iterator<char>());
        }
        catch (...) {
            std::cerr << "Problem reading gabac configuration " << configpath << " switching to default" << std::endl;
            config = defaultGabacConf;
        }

        FILE *fin_desc = fopen(file.c_str(), "rb");
        FILE *fout_desc = fopen((file + ".gabac").c_str(), "wb");

        if (!fin_desc) {
            throw std::runtime_error("Could not open " + file);
        }

        if (!fout_desc) {
            throw std::runtime_error("Could not open " + file + ".gabac");
        }

        gabac::FileInputStream fin(fin_desc);
        gabac::FileOutputStream fout(fout_desc);

        gabac::IOConfiguration
                ioconf = {&fin, &fout, 0, &std::cout, gabac::IOConfiguration::LogLevel::TRACE};

        gabac::EncodingConfiguration enConf(config);

        if (!decompress) {
            gabac::encode(ioconf, enConf);
            std::cout << "Sucessfully compressed ";
        } else {
            gabac::decode(ioconf, enConf);
            std::cout << "Sucessfully decompressed ";
        }


        fclose(fin_desc);
        fclose(fout_desc);

        std::cout << file << "\n(" << boost::filesystem::file_size(file) << " to\t";
        if (boost::filesystem::file_size(file) < 10000) {
            std::cout << "\t";
        }
        std::cout << boost::filesystem::file_size(file + ".gabac") << ")" << std::endl;

        std::remove(file.c_str());
        std::rename((file + ".gabac").c_str(), file.c_str());
    }
}


void packFile(const std::string& path, const std::string& file, FILE *fout){
    FILE *fin_desc = fopen((path + file).c_str(), "rb");
    if (!fin_desc) {
        throw std::runtime_error("Could not open " + (path + file));
    }
    uint64_t size = file.size();
    if (size == 0) {
        throw std::runtime_error("Cannot compress empty file");
    }
    if (fwrite(&size, sizeof(uint64_t), 1, fout) != 1) {
        fclose(fin_desc);
        throw std::runtime_error("Could not write to output file");
    }
    if (fwrite(file.c_str(), 1, size, fout) != size) {
        fclose(fin_desc);
        throw std::runtime_error("Could not write to output file");
    }

    fseek(fin_desc, 0, SEEK_END);
    size = ftell(fin_desc);
    if (fwrite(&size, sizeof(uint64_t), 1, fout) != 1) {
        fclose(fin_desc);
        throw std::runtime_error("Could not write to output file");
    }
    fseek(fin_desc, 0, SEEK_SET);

    uint64_t byteswritten = 0;
    std::vector<uint8_t> buffer(1000000);
    while (byteswritten < size) {
        uint64_t tmp = fread(buffer.data(), 1, std::min(buffer.size(), size - byteswritten), fin_desc);
        if (tmp != std::min(buffer.size(), size - byteswritten)) {
            fclose(fin_desc);
            throw std::runtime_error("Could not read from " + (path + file));
        }
        tmp = fwrite(buffer.data(), 1, tmp, fout);
        if (tmp != std::min(buffer.size(), size - byteswritten)) {
            fclose(fin_desc);
            throw std::runtime_error("Could not write to output file");
        }
        byteswritten += tmp;
    }

    fclose(fin_desc);
    std::cout << "Successfully packed " << (path + file) << std::endl;

}


std::string packFiles(const std::vector<std::string>& list, FILE *fout){
    std::string path;
    for (const auto& k : list) {
        size_t pos = k.find_last_of('/') + 1;
        path = k.substr(0, pos);
        packFile(path, k.substr(pos, std::string::npos), fout);
    }

    return path;
}

std::string unpackFile(const std::string& path, FILE *fin){
    uint64_t size;
    std::vector<uint8_t> buffer(1000000);
    if (fread(&size, sizeof(uint64_t), 1, fin) != 1) {
        throw std::runtime_error("Could not read from file #1");
    }
    std::string filename;
    filename.resize(size);
    if (fread((void *) filename.data(), sizeof(uint8_t), size, fin) != size) {
        throw std::runtime_error("Could not read from file #2");
    }
    if (fread(&size, sizeof(uint64_t), 1, fin) != 1) {
        throw std::runtime_error("Could not read from file #3");
    }

    FILE *fout = fopen((path + filename).c_str(), "wb");
    if (!fout) {
        throw std::runtime_error("Could not open output file #4");
    }

    uint64_t readBytes = 0;
    while (readBytes < size) {
        uint64_t tmp = fread(buffer.data(), 1, std::min(buffer.size(), size - readBytes), fin);
        if (tmp != std::min(buffer.size(), size - readBytes)) {
            fclose(fout);
            throw std::runtime_error("Could not read from input file #5");
        }
        readBytes += tmp;
        if (fwrite(buffer.data(), 1, tmp, fout) != tmp) {
            fclose(fout);
            throw std::runtime_error("Could not write to output file #6");
        }
    }

    fclose(fout);

    std::cout << "Successfully unpacked " << (path + filename) << std::endl;

    return path + filename;
}


std::vector<std::string> unpackFiles(const std::string& path, FILE *fin){
    uint64_t pos = ftell(fin);
    fseek(fin, 0, SEEK_END);
    uint64_t end = ftell(fin);
    fseek(fin, pos, SEEK_SET);

    std::vector<std::string> filelist;

    while (ftell(fin) < end) {
        filelist.push_back(unpackFile(path, fin));
    }

    return filelist;
}

std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> createMap(const std::vector<std::string>& filelist){
    std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> map;
    for (const auto& f : filelist) {
        size_t pos = std::string::npos;
        size_t tmp = 0;
        while ((tmp = f.find("ref_subseq_", tmp)) != std::string::npos) {
            pos = tmp;
            tmp++;
        }
        if (pos == std::string::npos) {
            continue;
        }
        pos += std::string("ref_subseq_").length();
        if (f.size() < pos + 5) {
            continue;
        }
        uint8_t j = f[pos] - '0';
        uint8_t k = f[pos + 2] - '0';
        uint8_t i = f[pos + 4] - '0';

        if (map.size() <= i) {
            map.resize(i + 1);
        }

        map[i][j][k] = f;
    }
    return map;
}

void decompression(
        const ProgramOptions& programOptions
){
    FILE *in = fopen(programOptions.inputFilePath.c_str(), "rb");
    if (!in) {
        throw std::runtime_error("Could not open input file");
    }
    std::string temp_dir;
    while (true) {
        std::string random_str = "tmp." + spring::random_string(10);
        temp_dir = "./" + random_str + '/';
        if (!boost::filesystem::exists(temp_dir)) {
            break;
        }
    }
    if (!boost::filesystem::create_directory(temp_dir)) {
        throw std::runtime_error("Cannot create temporary directory.");
    }
    std::cout << "Temporary directory: " << temp_dir << "\n";

    std::cout << "Starting decompression...\n";
    auto flist =
            unpackFiles(temp_dir, in);
    fclose(in);

    run_gabac(flist, true);

    spring::compression_params cp;
    FILE *cpfile = fopen((temp_dir + "cp.bin").c_str(), "rb");
    if (!cpfile) {
        throw std::runtime_error("Cannot open config file");
    }

    if (fread((uint8_t *) &cp, sizeof(spring::compression_params), 1, cpfile) != 1) {
        fclose(cpfile);
        throw std::runtime_error("Cannot read config");
    }
    fclose(cpfile);

    uint32_t num_blocks;
    if (!cp.paired_end) {
        num_blocks = 1 + (cp.num_reads - 1) / cp.num_reads_per_block;
    } else {
        num_blocks = 1 + (cp.num_reads / 2 - 1) / cp.num_reads_per_block;
    }

    spring::decompress(temp_dir, createMap(flist), num_blocks, cp.preserve_order, cp.paired_end);

    std::string outname = programOptions.inputFilePath;
    outname = outname.substr(0, outname.find_last_of('.'));
    outname += "_decompressed";

    int number = 0;
    while (boost::filesystem::exists(outname + std::to_string(number) + ".fastq")) {
        ++number;
    }
    outname += std::to_string(number) + ".fastq";

    std::rename((temp_dir + "decompressed.fastq").c_str(), outname.c_str());

    boost::filesystem::remove_all(temp_dir);
}

void generation(
        const ProgramOptions& programOptions
){
    generationFromFastq(programOptions);
    return;

    if (programOptions.inputFileType == "FASTA") {
        generationFromFasta(programOptions);
    } else if (programOptions.inputFileType == "FASTQ") {
        if (programOptions.readAlgorithm == "HARC") {
            auto generated_aus = generationFromFastq_SPRING(programOptions);

            std::vector<std::string> filelist;

            std::string path = generated_aus.getGeneratedAusRef().getRefAus().front().begin()->second.begin()->second;
            path = path.substr(0, path.find_last_of('/') + 1);

            boost::filesystem::path p(path);

            boost::filesystem::directory_iterator end_itr;

            std::remove((path + "read_order.bin").c_str());
            std::remove((path + "read_seq.txt").c_str());

            // cycle through the directory
            for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr) {
                // If it's not a directory, list it. If you want to list directories too, just remove this check.
                if (is_regular_file(itr->path())) {
                    // assign current file name to current_file and echo it out to the console.
                    std::string current_file = itr->path().string();
                    filelist.push_back(current_file);
                }
            }

            run_gabac(filelist, false);

            std::string outfile = (programOptions.inputFilePath
                                           .substr(0, programOptions.inputFilePath.find_last_of('.')) +
                                   ".genie");

            FILE *output = fopen(
                    outfile.c_str(), "wb"
            );
            if (!output) {
                throw std::runtime_error("Could not open output file");
            }

            packFiles(filelist, output);

            fclose(output);

            std::cout << "**** Finished ****" << std::endl;
            std::cout
                    << "Compressed "
                    << boost::filesystem::file_size(programOptions.inputFilePath)
                    << " to "
                    << boost::filesystem::file_size(outfile)
                    << ". Compression rate "
                    << float(boost::filesystem::file_size(outfile)) /
                       boost::filesystem::file_size(programOptions.inputFilePath) *
                       100
                    << "%"
                    << std::endl;

            boost::filesystem::remove_all(path);

        } else {
            generationFromFastq(programOptions);
        }
    } else if (programOptions.inputFileType == "SAM") {
        generationFromSam(programOptions);
    } else {
        throwRuntimeError("wrong input file type");
    }
}

void fastqSpringResultToFile(generated_aus generatedAus){
    MPEGGFileCreator fileCreator;
    DatasetGroup *datasetGroup1 = fileCreator.addDatasetGroup();

    datasetGroup1->addInternalReference("ref1", "seq1", generatedAus.getGeneratedAusRef());

    std::vector<Class_type> existing_classes;
    existing_classes.push_back(CLASS_P);
    existing_classes.push_back(CLASS_M);

    std::map<Class_type, std::vector<uint8_t>> descriptorsIdPerClass;
    descriptorsIdPerClass[CLASS_P].push_back(0);
    descriptorsIdPerClass[CLASS_P].push_back(1);
    descriptorsIdPerClass[CLASS_M].push_back(0);
    descriptorsIdPerClass[CLASS_M].push_back(1);


    std::map<uint16_t, std::map<Class_type, std::vector<AccessUnit>>> accessUnitsAligned;
    std::vector<AccessUnit> accessUnitsUnaligned;

    for (const auto& accessUnitEntry : generatedAus.getEncodedFastqAus()) {
        accessUnitsUnaligned.emplace_back(
                accessUnitEntry,
                0,
                0,
                CLASS_U,
                0,
                0
        );
    }

    fileCreator.write("testOutput0");
}


}  // namespace dsg
