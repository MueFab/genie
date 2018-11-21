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

#include "genie/exceptions.h"
#include "fileio/fasta_file_reader.h"
#include "fileio/fasta_record.h"
#include "fileio/fastq_file_reader.h"
#include "fileio/fastq_record.h"
#include "fileio/sam_file_reader.h"
#include "fileio/sam_record.h"
#include "coding/spring/spring.h"

namespace dsg {


static void generationFromFasta(
    const ProgramOptions& programOptions)
{
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
    const ProgramOptions& programOptions)
{
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "Descriptor stream generation from FASTQ file" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    // Initialize a FASTQ file reader.
    input::fastq::FastqFileReader fastqFileReader(programOptions.inputFilePath);

    // Second pass for HARC.


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
    if (!programOptions.inputFilePairPath.empty()) {
        std::cout << "Paired file:\n";
        // Initialize a FASTQ file reader.
        input::fastq::FastqFileReader fastqFileReader1(programOptions.inputFilePairPath);

        // Read FASTQ records in blocks of 10 records.
        size_t blockSize = 10;

        while (true) {
            std::vector<input::fastq::FastqRecord> fastqRecords;
            size_t numRecords = fastqFileReader1.readRecords(blockSize, &fastqRecords);
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
    }
}


static generated_aus generationFromFastq_SPRING(
    const ProgramOptions& programOptions)
{
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "Descriptor stream generation from FASTQ file" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    bool paired_end = false;
    // Initialize a FASTQ file reader.
    input::fastq::FastqFileReader fastqFileReader1(programOptions.inputFilePath);
    std::cout << "Calling SPRING" << std::endl;
    if (programOptions.inputFilePairPath.empty()) {
        return spring::generate_streams_SPRING(&fastqFileReader1, &fastqFileReader1, programOptions.numThreads, paired_end, programOptions.workingDirectory);
    } else {
        paired_end = true;
        input::fastq::FastqFileReader fastqFileReader2(programOptions.inputFilePairPath);
        return spring::generate_streams_SPRING(&fastqFileReader1, &fastqFileReader2, programOptions.numThreads, paired_end, programOptions.workingDirectory);
    }
}


static void generationFromSam(
    const ProgramOptions& programOptions)
{
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


void generation(
    const ProgramOptions& programOptions)
{
    if (programOptions.inputFileType == "FASTA") {
        generationFromFasta(programOptions);
    } else if (programOptions.inputFileType == "FASTQ") {
        if (programOptions.readAlgorithm == "HARC") {
            auto generated_aus = generationFromFastq_SPRING(programOptions);
            MPEGGFileCreator mpeggFileCreator;
            DatasetGroup* datasetGroup = mpeggFileCreator.addDatasetGroup();
            datasetGroup->addInternalReference("test","test",generated_aus.getGeneratedAusRef());

            std::vector<Class_type> existing_classes;
            existing_classes.push_back(CLASS_U);

            std::set<uint8_t> descriptorsUsed;
            for(const auto & auEntry : generated_aus.getEncodedFastqAus()){
                for(const auto & descriptorFileEntry : auEntry){
                    descriptorsUsed.insert(descriptorFileEntry.first);
                }
            }

            std::map<Class_type, std::vector<uint8_t>> descriptorsIdPerClass;
            for(uint8_t descriptorUsed : descriptorsUsed){
                descriptorsIdPerClass[CLASS_U].push_back(descriptorUsed);
            }

            std::map<uint16_t, std::map<Class_type, std::vector<AccessUnit>>> accessUnitsAligned;
            std::vector<AccessUnit> accessUnitsUnaligned;
            for(const auto & auEntry : generated_aus.getEncodedFastqAus()){
                accessUnitsUnaligned.push_back(AccessUnit(
                    auEntry,
                    0,
                    0,
                    CLASS_U,
                    0,
                    0
                ));
            }
            std::ofstream fakeParameters("fakePayload1");
            fakeParameters << "fakePayload1";
            fakeParameters.close();

            std::vector<std::string> parametersFilenames = {"fakePayload1"};

            datasetGroup->addDatasetData(
                    existing_classes,
                    descriptorsIdPerClass,
                    accessUnitsAligned,
                    accessUnitsUnaligned,
                    parametersFilenames
            );

            mpeggFileCreator.write("output.mpegg");


        }else {
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
    DatasetGroup* datasetGroup1 = fileCreator.addDatasetGroup();

    datasetGroup1->addInternalReference("ref1","seq1", generatedAus.getGeneratedAusRef());

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

    for(const auto& accessUnitEntry : generatedAus.getEncodedFastqAus()) {
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
