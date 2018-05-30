/** @file SAMFile.cc
 *  @brief This file contains the implementation of the SAMFile class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#include "IO/SAM/SAMFile.h"

#include <string.h>

#include <string>

#include "Common/Exceptions.h"
#include "Common/log.h"

namespace calq {

static void parseLine(char *fields[SAMRecord::NUM_FIELDS], char *line) {
    char *c = line;
    char *pc = c;
    int f = 0;

    while (true) {
        if (*c == '\t' || *c == '\0') {
            *c = '\0';
            if (f ==  0) fields[f] = pc;
            if (f ==  1) fields[f] = pc;
            if (f ==  2) fields[f] = pc;
            if (f ==  3) fields[f] = pc;
            if (f ==  4) fields[f] = pc;
            if (f ==  5) fields[f] = pc;
            if (f ==  6) fields[f] = pc;
            if (f ==  7) fields[f] = pc;
            if (f ==  8) fields[f] = pc;
            if (f ==  9) fields[f] = pc;
            if (f == 10) fields[f] = pc;
            if (f == 11) fields[f] = pc;
            f++;
            if (f == 12) { break; }
            pc = c + 1;
        }
        c++;
    }

    if (f == 11) { fields[f] = pc; }
}

SAMFile::SAMFile(const std::string &path, const Mode &mode)
    : File(path, mode),
      currentBlock(),
      header(""),
      line_(NULL),
      nrBlocksRead_(0),
      nrMappedRecordsRead_(0),
      nrUnmappedRecordsRead_(0),
      startTime_(std::chrono::steady_clock::now()) {
    if (path.empty() == true) {
        throwErrorException("path is empty");
    }
    if (mode != MODE_READ) {
        throwErrorException("Currently only MODE_READ supported");
    }

    // 1 million chars should be enough
    line_ = (char *)malloc(LINE_SIZE);
    if (line_ == NULL) {
        throwErrorException("malloc failed");
    }

    // Read SAM header
    size_t fpos = tell();
    for (;;) {
        fpos = tell();
        if (fgets(line_, LINE_SIZE, fp_) != NULL) {
            // Trim line
            size_t l = strlen(line_) - 1;
            while (l && (line_[l] == '\r' || line_[l] == '\n')) {
                line_[l--] = '\0';
            }

            if (line_[0] == '@') {
                header += line_;
                header += "\n";
            } else {
                break;
            }
        } else {
            throwErrorException("Could not read SAM header");
        }
    }
    seek(fpos);  // rewind to the begin of the alignment section
    if (header.empty() == true) {
        CALQ_LOG("No SAM header found");
    }
}

SAMFile::~SAMFile(void) {
    free(line_);
}

size_t SAMFile::nrBlocksRead(void) const {
    return nrBlocksRead_;
}

size_t SAMFile::nrMappedRecordsRead() const {
    return nrMappedRecordsRead_;
}

size_t SAMFile::nrUnmappedRecordsRead() const {
    return nrUnmappedRecordsRead_;
}

size_t SAMFile::nrRecordsRead() const {
    return (nrMappedRecordsRead_ + nrUnmappedRecordsRead_);
}

size_t SAMFile::readBlock(const size_t &blockSize) {
    if (blockSize < 1) {
        throwErrorException("blockSize must be greater than zero");
    }

    currentBlock.reset();

    std::string rnamePrev("");
    uint32_t posPrev = 0;

    for (size_t i = 0; i < blockSize; i++) {
        size_t fpos = tell();
        if (fgets(line_, LINE_SIZE, fp_) != NULL) {
            // Trim line
            size_t l = strlen(line_) - 1;
            while (l && (line_[l] == '\r' || line_[l] == '\n')) {
                line_[l--] = '\0';
            }

            // Parse line and construct samRecord
            char *fields[SAMRecord::NUM_FIELDS];
            parseLine(fields, line_);
            SAMRecord samRecord(fields);

            if (samRecord.isMapped() == true) {
                if (rnamePrev.empty() == true) {
                    // This is the first mapped record in this block; just store
                    // its RNAME and POS and add it to the current block
                    rnamePrev = samRecord.rname;
                    posPrev = samRecord.pos;
                    currentBlock.records.push_back(samRecord);
                    currentBlock.nrMappedRecords_++;
                } else {
                    // We already have a mapped record in this block
                    if (rnamePrev == samRecord.rname) {
                        // RNAME didn't change, check POS
                        if (samRecord.pos >= posPrev) {
                            // Everything fits, just update posPrev and push
                            // the samRecord to the current block
                            posPrev = samRecord.pos;
                            currentBlock.records.push_back(samRecord);
                            currentBlock.nrMappedRecords_++;
                        } else {
                            throwErrorException("SAM file is not sorted");
                        }
                    } else {
                        // RNAME changed, seek back and break
                        seek(fpos);
                        CALQ_LOG("RNAME changed - read only %zu record(s) (%zu requested)", currentBlock.nrRecords(), blockSize);
                        break;
                    }
                }
            } else {
                currentBlock.records.push_back(samRecord);
                currentBlock.nrUnmappedRecords_++;
            }
        } else {
            CALQ_LOG("Truncated block - read only %zu record(s) (%zu requested) - reached EOF", currentBlock.nrRecords(), blockSize);
            break;
        }
    }

    if (currentBlock.nrRecords() > 0) {
        nrBlocksRead_++;
        nrMappedRecordsRead_ += currentBlock.nrMappedRecords();
        nrUnmappedRecordsRead_ += currentBlock.nrUnmappedRecords();
    }

    auto elapsedTime = std::chrono::steady_clock::now() - startTime_;
    auto elapsedTimeS = std::chrono::duration_cast<std::chrono::seconds>(elapsedTime).count();
    double elapsedTimeM = (double)elapsedTimeS / (double)60;
    double processedPercentage = ((double)tell() / (double)size()) * 100;
    auto remainingPercentage = 100 - processedPercentage;
    CALQ_LOG("Processed: %.2f%% (elapsed: %.2f m), remaining: %.2f%% (~%.2f m)",
             processedPercentage,
             elapsedTimeM,
             remainingPercentage,
             elapsedTimeM * (remainingPercentage/processedPercentage));

    return currentBlock.nrRecords();
}

}  // namespace calq

