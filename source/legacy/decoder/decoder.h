#include <iostream>
#include <map>
#include <vector>
#include "access-unit.h"
#include "data-unit.h"
#include "definitions.h"
#include "expandable-array.h"
#include "parameter-set.h"
#include "raw-reference.h"
#include "read.h"

#ifndef DECODER_H
#define DECODER_H

void decodeClassId(AccessUnit &AU, std::vector<ParameterSet> collection, RawReference RR, uint64_t &previousMappingPos0,
                   uint64_t &classId) {
    classId = AU.access_unit_header.AU_type;
}

void getDecodedSymbols(AccessUnit &AU, std::vector<ParameterSet> collection, RawReference RR,
                       uint64_t &previousMappingPos0,
                       expandable_array<expandable_array<expandable_array<uint64_t> > > &decoded_symbols) {
    decoded_symbols = AU.decoded_symbols;
}

void decodeNumberOfSegmentVariables(AccessUnit &AU, std::vector<ParameterSet> collection, RawReference RR,
                                    uint64_t &previousMappingPos0,
                                    expandable_array<expandable_array<expandable_array<uint64_t> > > decoded_symbols,
                                    uint64_t &numberOfTemplateSegments, uint64_t &numberOfRecordSegments,
                                    uint64_t &numberOfAlignedRecordSegments, uint64_t &unpairedRead) {
    ParameterSet PS = AU.access_unit_header.getParameterSet(collection);

    uint64_t classId = AU.access_unit_header.AU_type;
    uint64_t AU_start_position = AU.access_unit_header.AU_start_position;
    uint64_t seqId = AU.access_unit_header.sequence_ID;
    int64_t reads_count = AU.access_unit_header.reads_count;

    uint64_t alphabet_ID = PS.encoding_parameters.alphabet_ID;
    uint64_t read_length = PS.encoding_parameters.read_length;
    uint64_t cr_alg_ID = PS.encoding_parameters.parameter_set_crps.cr_alg_ID;
    uint64_t num_groups = PS.encoding_parameters.num_groups;
    bool spliced_reads_flag = PS.encoding_parameters.spliced_reads_flag;
    bool multiple_alignments_flag = PS.encoding_parameters.multiple_alignments_flag;
    bool crps_flag = PS.encoding_parameters.crps_flag;
    uint64_t as_depth = PS.encoding_parameters.as_depth;
    uint64_t qv_depth = PS.encoding_parameters.qv_depth;
    bool qv_reverse_flag = PS.encoding_parameters.qv_reverse_flag;
    uint64_t qv_num_codebooks_total = PS.encoding_parameters.parameter_set_qvps[classId].qv_num_codebooks_total;

    numberOfTemplateSegments = PS.encoding_parameters.number_of_template_segments_minus1 + 1;

    if (numberOfTemplateSegments == 1)
        numberOfRecordSegments = 1;
    else if (classId == CLASS_HM)
        numberOfRecordSegments = 2;
    else if (subsequence0(8, 0) == 0)
        numberOfRecordSegments = 2;
    else
        numberOfRecordSegments = 1;  // 2.1

    reads_count -= numberOfRecordSegments;

    if (classId == CLASS_HM)
        numberOfAlignedRecordSegments = 1;
    else if (classId == CLASS_U)
        numberOfAlignedRecordSegments = 0;
    else
        numberOfAlignedRecordSegments = numberOfRecordSegments;  // 2.2

    if (classId == CLASS_HM)
        unpairedRead = 0;
    else if (numberOfTemplateSegments == 1 || subsequence0(8, 0) == 5 || subsequence0(8, 0) == 6)
        unpairedRead = 1;
    else
        unpairedRead = 0;  // 2.3
}

void decodeCLIPS(AccessUnit &AU, std::vector<ParameterSet> collection, RawReference RR, uint64_t &previousMappingPos0,
                 expandable_array<expandable_array<expandable_array<uint64_t> > > decoded_symbols,
                 uint64_t numberOfAlignedRecordSegments, expandable_array<expandable_array<std::string> > &softClips,
                 expandable_array<expandable_array<uint64_t> > &softClipSizes,
                 expandable_array<expandable_array<uint64_t> > &hardClips, expandable_array<std::string> &S,
                 uint64_t &currentRecordCount) {
    ParameterSet PS = AU.access_unit_header.getParameterSet(collection);

    uint64_t classId = AU.access_unit_header.AU_type;
    uint64_t AU_start_position = AU.access_unit_header.AU_start_position;
    uint64_t seqId = AU.access_unit_header.sequence_ID;
    int64_t reads_count = AU.access_unit_header.reads_count;

    uint64_t alphabet_ID = PS.encoding_parameters.alphabet_ID;
    uint64_t read_length = PS.encoding_parameters.read_length;
    uint64_t cr_alg_ID = PS.encoding_parameters.parameter_set_crps.cr_alg_ID;
    uint64_t num_groups = PS.encoding_parameters.num_groups;
    bool spliced_reads_flag = PS.encoding_parameters.spliced_reads_flag;
    bool multiple_alignments_flag = PS.encoding_parameters.multiple_alignments_flag;
    bool crps_flag = PS.encoding_parameters.crps_flag;
    uint64_t as_depth = PS.encoding_parameters.as_depth;
    uint64_t qv_depth = PS.encoding_parameters.qv_depth;
    bool qv_reverse_flag = PS.encoding_parameters.qv_reverse_flag;
    uint64_t qv_num_codebooks_total = PS.encoding_parameters.parameter_set_qvps[classId].qv_num_codebooks_total;

    initialize_supported_symbols(S);

    for (size_t i = 0; i < numberOfAlignedRecordSegments; ++i) {
        for (size_t j = 0; j < 2; ++j) {
            softClips[i][j] = "";
            softClipSizes[i][j] = 0;
            hardClips[i][j] = 0;
        }
    }

    if (classId == CLASS_I || classId == CLASS_HM) {
        if (AU.j[5][0] < _subsequence0(5).size() && currentRecordCount == subsequence0(5, 0)) {
            bool end = 0;

            uint64_t j;
            uint64_t segmentIdx;
            uint64_t leftRightIdx;

            do {
                if (subsequence1(5, 1) <= 3) {
                    j = 0;
                    segmentIdx = subsequence1(5, 1) >> 1;
                    leftRightIdx = subsequence1(5, 1) & 1;

                    do {
                        softClips[segmentIdx][leftRightIdx][j] = S[alphabet_ID][subsequence2(5, 2)];
                        AU.j[5][2]++;
                        j++;
                    } while (subsequence2(5, 2) != S[alphabet_ID].size());

                    AU.j[5][2]++;
                    softClipSizes[segmentIdx][leftRightIdx] = j;
                } else if (subsequence1(5, 1) <= 7) {
                    segmentIdx = (subsequence1(5, 1) - 4) >> 1;
                    leftRightIdx = (subsequence1(5, 1) - 5) & 1;
                    hardClips[segmentIdx][leftRightIdx] = subsequence3(5, 3);
                    AU.j[5][3]++;
                } else if (subsequence1(5, 1) == 8)
                    end = 1;

                AU.j[5][1]++;
            } while (!end);

            AU.j[5][0]++;
        }

        currentRecordCount++;
    }  // 3
}

void decodeRLEN(AccessUnit &AU, std::vector<ParameterSet> collection, RawReference RR, uint64_t &previousMappingPos0,
                expandable_array<expandable_array<expandable_array<uint64_t> > > decoded_symbols,
                uint64_t numberOfRecordSegments, uint64_t numberOfAlignedRecordSegments,
                expandable_array<expandable_array<uint64_t> > hardClips, expandable_array<uint64_t> &readLength,
                expandable_array<uint64_t> &numberOfSplicedSeg,
                expandable_array<expandable_array<uint64_t> > &splicedSegLength) {
    ParameterSet PS = AU.access_unit_header.getParameterSet(collection);

    uint64_t classId = AU.access_unit_header.AU_type;
    uint64_t AU_start_position = AU.access_unit_header.AU_start_position;
    uint64_t seqId = AU.access_unit_header.sequence_ID;
    int64_t reads_count = AU.access_unit_header.reads_count;

    uint64_t alphabet_ID = PS.encoding_parameters.alphabet_ID;
    uint64_t read_length = PS.encoding_parameters.read_length;
    uint64_t cr_alg_ID = PS.encoding_parameters.parameter_set_crps.cr_alg_ID;
    uint64_t num_groups = PS.encoding_parameters.num_groups;
    bool spliced_reads_flag = PS.encoding_parameters.spliced_reads_flag;
    bool multiple_alignments_flag = PS.encoding_parameters.multiple_alignments_flag;
    bool crps_flag = PS.encoding_parameters.crps_flag;
    uint64_t as_depth = PS.encoding_parameters.as_depth;
    uint64_t qv_depth = PS.encoding_parameters.qv_depth;
    bool qv_reverse_flag = PS.encoding_parameters.qv_reverse_flag;
    uint64_t qv_num_codebooks_total = PS.encoding_parameters.parameter_set_qvps[classId].qv_num_codebooks_total;

    if (read_length == 0) {
        for (size_t i = 0; i < numberOfRecordSegments; ++i) {
            readLength[i] = subsequence0(7, 0) + 1;
            AU.j[7][0]++;
        }
    } else {
        for (size_t i = 0; i < numberOfRecordSegments; ++i) {
            if (classId == CLASS_I)
                readLength[i] = read_length - hardClips[i][0] - hardClips[i][1];
            else if (classId == CLASS_HM && i == 0)
                readLength[i] = read_length - hardClips[0][0] - hardClips[0][1];
            else
                readLength[i] = read_length;
        }
    }

    for (size_t i = 0; i < numberOfRecordSegments; ++i) {
        numberOfSplicedSeg[i] = 1;
        splicedSegLength[i][0] = readLength[i];
    }

    if (spliced_reads_flag && (classId == CLASS_I || classId == CLASS_HM)) {
        uint64_t remainingLen;
        uint64_t spliceLen;
        uint64_t j;

        for (size_t i = 0; i < numberOfAlignedRecordSegments; ++i) {
            if (read_length == 0) {
                readLength[i] = subsequence0(7, 0) + 1;
                AU.j[7][0]++;
            }

            remainingLen = readLength[i];
            j = 0;

            do {
                spliceLen = subsequence0(7, 0);
                AU.j[7][0]++;
                remainingLen -= spliceLen;
                splicedSegLength[i][j++] = spliceLen;
            } while (remainingLen > 0);

            numberOfSplicedSeg[i] = j;
        }
    }  // 4
}

void decodeMMAP(AccessUnit &AU, std::vector<ParameterSet> collection, RawReference RR, uint64_t &previousMappingPos0,
                expandable_array<expandable_array<expandable_array<uint64_t> > > decoded_symbols,
                uint64_t numberOfRecordSegments, uint64_t unpairedRead, uint64_t &numberOfAlignments,
                expandable_array<uint64_t> &numberOfSegmentAlignments,
                expandable_array<uint64_t> &numberOfAlignmentPairs,
                expandable_array<expandable_array<uint64_t> > &alignPtr, uint64_t &moreAlignments,
                uint64_t &moreAlignmentsNextPos, uint64_t &moreAlignmentsNextSeqId) {
    ParameterSet PS = AU.access_unit_header.getParameterSet(collection);

    uint64_t classId = AU.access_unit_header.AU_type;
    uint64_t AU_start_position = AU.access_unit_header.AU_start_position;
    uint64_t seqId = AU.access_unit_header.sequence_ID;
    int64_t reads_count = AU.access_unit_header.reads_count;

    uint64_t alphabet_ID = PS.encoding_parameters.alphabet_ID;
    uint64_t read_length = PS.encoding_parameters.read_length;
    uint64_t cr_alg_ID = PS.encoding_parameters.parameter_set_crps.cr_alg_ID;
    uint64_t num_groups = PS.encoding_parameters.num_groups;
    bool spliced_reads_flag = PS.encoding_parameters.spliced_reads_flag;
    bool multiple_alignments_flag = PS.encoding_parameters.multiple_alignments_flag;
    bool crps_flag = PS.encoding_parameters.crps_flag;
    uint64_t as_depth = PS.encoding_parameters.as_depth;
    uint64_t qv_depth = PS.encoding_parameters.qv_depth;
    bool qv_reverse_flag = PS.encoding_parameters.qv_reverse_flag;
    uint64_t qv_num_codebooks_total = PS.encoding_parameters.parameter_set_qvps[classId].qv_num_codebooks_total;

    if (classId != CLASS_U) {
        if (multiple_alignments_flag == 0)
            numberOfSegmentAlignments[0] = 1;
        else {
            numberOfSegmentAlignments[0] = subsequence0(10, 0);
            AU.j[10][0]++;
        }
    } else
        numberOfSegmentAlignments[0] = 0;

    moreAlignments = 0;

    if (unpairedRead || classId == CLASS_HM) {
        numberOfAlignments = numberOfSegmentAlignments[0];

        for (size_t i = 0; i < numberOfAlignments; ++i) alignPtr[i][0] = i;
    } else if (classId == CLASS_U) {
        if (numberOfRecordSegments > 1) numberOfSegmentAlignments[1] = 0;
        numberOfAlignments = 0;
    } else {
        numberOfSegmentAlignments[1] = 0;
        uint64_t k, i, j, ptr;
        k = i = 0;

        while (i < numberOfSegmentAlignments[0]) {
            if (multiple_alignments_flag == 0)
                numberOfAlignmentPairs[i] = 1;
            else {
                numberOfAlignmentPairs[i] = subsequence0(10, 0);
                AU.j[10][0]++;
            }

            j = 0;

            while (j < numberOfAlignmentPairs[i]) {
                if (i != 0) {
                    ptr = subsequence1(10, 1);
                    AU.j[10][1]++;
                } else
                    ptr = 0;

                alignPtr[k][1] = numberOfSegmentAlignments[1] - ptr;
                alignPtr[k][0] = i;

                if (ptr == 0) numberOfSegmentAlignments[1]++;

                j++;
                k++;
            }

            i++;
        }

        numberOfAlignments = k;
    }

    if (multiple_alignments_flag == 1 && classId != CLASS_U && subsequence2(10, 2)) {
        moreAlignments = 1;
        moreAlignmentsNextSeqId = subsequence3(10, 3);
        moreAlignmentsNextPos = subsequence4(10, 4);
        AU.j[10][3]++;
        AU.j[10][4]++;
    }

    AU.j[10][2]++;  // 5
}

void decodePOS(AccessUnit &AU, std::vector<ParameterSet> collection, RawReference RR, uint64_t &previousMappingPos0,
               expandable_array<expandable_array<expandable_array<uint64_t> > > decoded_symbols,
               expandable_array<uint64_t> numberOfSegmentAlignments,
               expandable_array<expandable_array<uint64_t> > &mappingPos) {
    ParameterSet PS = AU.access_unit_header.getParameterSet(collection);

    uint64_t classId = AU.access_unit_header.AU_type;
    uint64_t AU_start_position = AU.access_unit_header.AU_start_position;
    uint64_t seqId = AU.access_unit_header.sequence_ID;
    int64_t reads_count = AU.access_unit_header.reads_count;

    uint64_t alphabet_ID = PS.encoding_parameters.alphabet_ID;
    uint64_t read_length = PS.encoding_parameters.read_length;
    uint64_t cr_alg_ID = PS.encoding_parameters.parameter_set_crps.cr_alg_ID;
    uint64_t num_groups = PS.encoding_parameters.num_groups;
    bool spliced_reads_flag = PS.encoding_parameters.spliced_reads_flag;
    bool multiple_alignments_flag = PS.encoding_parameters.multiple_alignments_flag;
    bool crps_flag = PS.encoding_parameters.crps_flag;
    uint64_t as_depth = PS.encoding_parameters.as_depth;
    uint64_t qv_depth = PS.encoding_parameters.qv_depth;
    bool qv_reverse_flag = PS.encoding_parameters.qv_reverse_flag;
    uint64_t qv_num_codebooks_total = PS.encoding_parameters.parameter_set_qvps[classId].qv_num_codebooks_total;

    if (AU.j[0][0] > 0)
        mappingPos[0][0] = previousMappingPos0 + subsequence0(0, 0);
    else
        mappingPos[0][0] = AU_start_position + subsequence0(0, 0);

    previousMappingPos0 = mappingPos[0][0];

    for (size_t i = 1; i < numberOfSegmentAlignments[0]; ++i) {
        mappingPos[i][0] = mappingPos[i - 1][0] + subsequence1(0, 1);
        AU.j[0][1]++;
    }

    AU.j[0][0]++;  // 6
}

void decodePAIR(AccessUnit &AU, std::vector<ParameterSet> collection, RawReference RR, uint64_t &previousMappingPos0,
                expandable_array<expandable_array<expandable_array<uint64_t> > > decoded_symbols,
                uint64_t numberOfTemplateSegments, expandable_array<expandable_array<uint64_t> > mappingPos,
                uint64_t &read1First, uint64_t &delta, expandable_array<expandable_array<uint64_t> > &splitMate,
                expandable_array<expandable_array<uint64_t> > &splicedSegMappingPos,
                expandable_array<expandable_array<uint64_t> > &mateSeqId, expandable_array<uint64_t> &mateRecordIndex,
                expandable_array<uint64_t> &mateAuId) {
    ParameterSet PS = AU.access_unit_header.getParameterSet(collection);

    uint64_t classId = AU.access_unit_header.AU_type;
    uint64_t AU_start_position = AU.access_unit_header.AU_start_position;
    uint64_t seqId = AU.access_unit_header.sequence_ID;
    int64_t reads_count = AU.access_unit_header.reads_count;

    uint64_t alphabet_ID = PS.encoding_parameters.alphabet_ID;
    uint64_t read_length = PS.encoding_parameters.read_length;
    uint64_t cr_alg_ID = PS.encoding_parameters.parameter_set_crps.cr_alg_ID;
    uint64_t num_groups = PS.encoding_parameters.num_groups;
    bool spliced_reads_flag = PS.encoding_parameters.spliced_reads_flag;
    bool multiple_alignments_flag = PS.encoding_parameters.multiple_alignments_flag;
    bool crps_flag = PS.encoding_parameters.crps_flag;
    uint64_t as_depth = PS.encoding_parameters.as_depth;
    uint64_t qv_depth = PS.encoding_parameters.qv_depth;
    bool qv_reverse_flag = PS.encoding_parameters.qv_reverse_flag;
    uint64_t qv_num_codebooks_total = PS.encoding_parameters.parameter_set_qvps[classId].qv_num_codebooks_total;

    splitMate[0][0] = 0;

    if (classId != CLASS_HM) {
        for (size_t i = 1; i < numberOfTemplateSegments; ++i) {
            if (subsequence0(8, 0) == 0) {
                splitMate[0][i] = 0;
                if (classId != CLASS_U) {
                    read1First = (subsequence1(8, 1) & 0x0001) ? 0 : 1;
                    delta = subsequence1(8, 1) >> 1;
                    mappingPos[0][i] = mappingPos[0][0] + delta;
                    mateSeqId[0][i] = seqId;
                    AU.j[8][1]++;
                } else {
                    read1First = 1;
                    mateAuId[1] = minus1;
                    mateRecordIndex[i] = minus1;
                }
            } else if (subsequence0(8, 0) == 1) {
                splitMate[0][i] = 1;
                read1First = 0;
                if (classId != CLASS_U) {
                    mappingPos[0][i] = subsequence2(8, 2);
                    mateSeqId[0][i] = seqId;
                } else {
                    mateAuId[1] = minus1;
                    mateRecordIndex[i] = subsequence2(8, 2);
                }
                AU.j[8][2]++;
            } else if (subsequence0(8, 0) == 2) {
                splitMate[0][i] = 1;
                read1First = 1;
                if (classId != CLASS_U) {
                    mappingPos[0][i] = subsequence3(8, 3);
                    mateSeqId[0][i] = seqId;
                } else {
                    mateAuId[1] = minus1;
                    mateRecordIndex[i] = subsequence2(8, 3);
                }
                AU.j[8][3]++;
            } else if (subsequence0(8, 0) == 3) {
                splitMate[0][i] = 1;
                read1First = 0;
                if (classId != CLASS_U) {
                    mateSeqId[0][i] = subsequence4(8, 4);
                    mappingPos[0][i] = subsequence6(8, 6);
                } else {
                    mateAuId[1] = subsequence4(8, 4);
                    mateRecordIndex[1] = subsequence6(8, 6);
                }
                AU.j[8][4]++;
                AU.j[8][6]++;
            } else if (subsequence0(8, 0) == 4) {
                splitMate[0][i] = 1;
                read1First = 1;
                if (classId != CLASS_U) {
                    mateSeqId[0][i] = subsequence5(8, 5);
                    mappingPos[0][i] = subsequence7(8, 7);
                } else {
                    mateAuId[1] = subsequence5(8, 5);
                    mateRecordIndex[1] = subsequence7(8, 7);
                }
                AU.j[8][5]++;
                AU.j[8][7]++;
            } else if (subsequence0(8, 0) == 5) {
                splitMate[0][i] = 2;
                read1First = 1;
            } else if (subsequence0(8, 0) == 6) {
                splitMate[0][i] = 2;
                read1First = 0;
            }

            AU.j[8][0]++;
        }

    }  // 7
}

void decodeRCOMP(AccessUnit &AU, std::vector<ParameterSet> collection, RawReference RR, uint64_t &previousMappingPos0,
                 expandable_array<expandable_array<expandable_array<uint64_t> > > decoded_symbols,
                 expandable_array<expandable_array<uint64_t> > mappingPos, uint64_t numberOfRecordSegments,
                 uint64_t numberOfAlignedRecordSegments, expandable_array<uint64_t> numberOfSegmentAlignments,
                 expandable_array<expandable_array<uint64_t> > splitMate, expandable_array<uint64_t> numberOfSplicedSeg,
                 expandable_array<expandable_array<expandable_array<uint64_t> > > &reverseComp,
                 expandable_array<uint64_t> &numberOfSegmentMappings, uint64_t &numberOfMappedRecordSegments) {
    ParameterSet PS = AU.access_unit_header.getParameterSet(collection);

    uint64_t classId = AU.access_unit_header.AU_type;
    uint64_t AU_start_position = AU.access_unit_header.AU_start_position;
    uint64_t seqId = AU.access_unit_header.sequence_ID;
    int64_t reads_count = AU.access_unit_header.reads_count;

    uint64_t alphabet_ID = PS.encoding_parameters.alphabet_ID;
    uint64_t read_length = PS.encoding_parameters.read_length;
    uint64_t cr_alg_ID = PS.encoding_parameters.parameter_set_crps.cr_alg_ID;
    uint64_t num_groups = PS.encoding_parameters.num_groups;
    bool spliced_reads_flag = PS.encoding_parameters.spliced_reads_flag;
    bool multiple_alignments_flag = PS.encoding_parameters.multiple_alignments_flag;
    bool crps_flag = PS.encoding_parameters.crps_flag;
    uint64_t as_depth = PS.encoding_parameters.as_depth;
    uint64_t qv_depth = PS.encoding_parameters.qv_depth;
    bool qv_reverse_flag = PS.encoding_parameters.qv_reverse_flag;
    uint64_t qv_num_codebooks_total = PS.encoding_parameters.parameter_set_qvps[classId].qv_num_codebooks_total;

    if (classId == CLASS_U && crps_flag != 0 && (cr_alg_ID == 2 || cr_alg_ID == 4)) {
        numberOfMappedRecordSegments = numberOfRecordSegments;
        for (size_t i = 0; i < numberOfRecordSegments; ++i) numberOfSegmentMappings[i] = 1;
    } else {
        numberOfMappedRecordSegments = numberOfAlignedRecordSegments;
        numberOfSegmentMappings =
            expandable_array<uint64_t>(numberOfSegmentAlignments.begin(), numberOfSegmentAlignments.end());
    }

    for (size_t i = 0; i < numberOfMappedRecordSegments; ++i) {
        for (size_t j = 0; j < numberOfSegmentMappings[i]; ++j) {
            if (splitMate[j][i] == 0) {
                for (size_t k = 0; k < numberOfSplicedSeg[i]; ++k) {
                    reverseComp[k][j][i] = subsequence0(1, 0);
                    AU.j[1][0]++;
                }
            }
        }
    }  // 8
}

void decodeRGROUP(AccessUnit &AU, std::vector<ParameterSet> collection, RawReference RR, uint64_t &previousMappingPos0,
                  expandable_array<expandable_array<expandable_array<uint64_t> > > decoded_symbols,
                  uint64_t &readGroupId, bool &has_readGroupId) {
    ParameterSet PS = AU.access_unit_header.getParameterSet(collection);

    uint64_t classId = AU.access_unit_header.AU_type;
    uint64_t AU_start_position = AU.access_unit_header.AU_start_position;
    uint64_t seqId = AU.access_unit_header.sequence_ID;
    int64_t reads_count = AU.access_unit_header.reads_count;

    uint64_t alphabet_ID = PS.encoding_parameters.alphabet_ID;
    uint64_t read_length = PS.encoding_parameters.read_length;
    uint64_t cr_alg_ID = PS.encoding_parameters.parameter_set_crps.cr_alg_ID;
    uint64_t num_groups = PS.encoding_parameters.num_groups;
    bool spliced_reads_flag = PS.encoding_parameters.spliced_reads_flag;
    bool multiple_alignments_flag = PS.encoding_parameters.multiple_alignments_flag;
    bool crps_flag = PS.encoding_parameters.crps_flag;
    uint64_t as_depth = PS.encoding_parameters.as_depth;
    uint64_t qv_depth = PS.encoding_parameters.qv_depth;
    bool qv_reverse_flag = PS.encoding_parameters.qv_reverse_flag;
    uint64_t qv_num_codebooks_total = PS.encoding_parameters.parameter_set_qvps[classId].qv_num_codebooks_total;

    if (num_groups > 0) {
        has_readGroupId = true;
        readGroupId = subsequence0(13, 0);
        AU.j[13][0]++;
    }  // 9
}

void decodeSplicedSequence(AccessUnit &AU, std::vector<ParameterSet> collection, RawReference RR,
                           uint64_t &previousMappingPos0,
                           expandable_array<expandable_array<expandable_array<uint64_t> > > decoded_symbols,
                           uint64_t numberOfMappedRecordSegments, expandable_array<uint64_t> numberOfSplicedSeg,
                           expandable_array<expandable_array<uint64_t> > mappingPos,
                           expandable_array<expandable_array<uint64_t> > softClipSizes,
                           expandable_array<expandable_array<uint64_t> > splicedSegLength,
                           expandable_array<expandable_array<std::string> > &splicedSequence, int64_t &pRef,
                           uint64_t &mappedLength) {
    ParameterSet PS = AU.access_unit_header.getParameterSet(collection);

    uint64_t classId = AU.access_unit_header.AU_type;
    uint64_t AU_start_position = AU.access_unit_header.AU_start_position;
    uint64_t seqId = AU.access_unit_header.sequence_ID;
    int64_t reads_count = AU.access_unit_header.reads_count;

    uint64_t alphabet_ID = PS.encoding_parameters.alphabet_ID;
    uint64_t read_length = PS.encoding_parameters.read_length;
    uint64_t cr_alg_ID = PS.encoding_parameters.parameter_set_crps.cr_alg_ID;
    uint64_t num_groups = PS.encoding_parameters.num_groups;
    bool spliced_reads_flag = PS.encoding_parameters.spliced_reads_flag;
    bool multiple_alignments_flag = PS.encoding_parameters.multiple_alignments_flag;
    bool crps_flag = PS.encoding_parameters.crps_flag;
    uint64_t as_depth = PS.encoding_parameters.as_depth;
    uint64_t qv_depth = PS.encoding_parameters.qv_depth;
    bool qv_reverse_flag = PS.encoding_parameters.qv_reverse_flag;
    uint64_t qv_num_codebooks_total = PS.encoding_parameters.parameter_set_qvps[classId].qv_num_codebooks_total;

    std::cout << "number_of_mapped_record_segments: " << numberOfMappedRecordSegments << std::endl;

    for (size_t i = 0; i < numberOfMappedRecordSegments; ++i) {
        for (size_t j = 0; j < numberOfSplicedSeg[i]; ++j) {
            // pRef = splicedSegMappingPos[i][j] - RR.seq_start[seqId];
            pRef = mappingPos[j][i] - RR.seq_start[seqId];
            mappedLength = splicedSegLength[i][j];

            if (classId == CLASS_I || classId == CLASS_HM) {
                if (j == 0) mappedLength -= softClipSizes[i][0];
                if (j == numberOfSplicedSeg[i] - 1) mappedLength -= softClipSizes[i][1];
            }

            splicedSequence[i][j] = RR.ref_sequence[seqId].substr(pRef, mappedLength);
            std::cout << "splicedSequence[ " << i << ", " << j << " ]: " << splicedSequence[i][j] << std::endl;
            if (classId == CLASS_N) {                               /* processSplSeqN(i, j) */
            } else if (classId == CLASS_M) {                        /* processSplSegM(i, j) */
            } else if (classId == CLASS_I || classId == CLASS_HM) { /* processSplSegI(i, j) */
            }
        }
    }  // 14 (10.5)

    std::cout << "class_ID: " << classId << std::endl;
    std::cout << "seq_ID: " << seqId << std::endl;
    std::cout << "as_depth: " << as_depth << std::endl;
    std::cout << "qv_depth: " << qv_depth << std::endl;
}

void decode(AccessUnit &AU, std::vector<ParameterSet> collection, RawReference RR, uint64_t &previousMappingPos0) {
    uint64_t classId;
    uint64_t currentRecordCount = 0;
    uint64_t reads_count = AU.access_unit_header.reads_count;
    expandable_array<expandable_array<expandable_array<uint64_t> > > decoded_symbols;

    decodeClassId(AU, collection, RR, previousMappingPos0, classId);

    if (classId != CLASS_P) return;  // TODO

    getDecodedSymbols(AU, collection, RR, previousMappingPos0, decoded_symbols);

    // while(AU.j[0][0] < decoded_symbols[0][0].size()) {
    while (reads_count > 0) {
        uint64_t numberOfTemplateSegments;
        uint64_t numberOfRecordSegments;
        uint64_t numberOfAlignedRecordSegments;
        uint64_t unpairedRead;
        expandable_array<expandable_array<std::string> > softClips;
        expandable_array<expandable_array<uint64_t> > softClipSizes;
        expandable_array<expandable_array<uint64_t> > hardClips;
        expandable_array<std::string> S;
        expandable_array<uint64_t> readLength;
        expandable_array<uint64_t> numberOfSplicedSeg;
        expandable_array<expandable_array<uint64_t> > splicedSegLength;
        uint64_t numberOfAlignments;
        expandable_array<uint64_t> numberOfSegmentAlignments;
        expandable_array<uint64_t> numberOfAlignmentPairs;
        expandable_array<expandable_array<uint64_t> > alignPtr;
        uint64_t moreAlignments;
        uint64_t moreAlignmentsNextPos;
        uint64_t moreAlignmentsNextSeqId;
        expandable_array<expandable_array<uint64_t> > mappingPos;
        uint64_t read1First;
        uint64_t delta;
        expandable_array<expandable_array<uint64_t> > splitMate;
        expandable_array<expandable_array<uint64_t> > splicedSegMappingPos;
        expandable_array<expandable_array<uint64_t> > mateSeqId;
        expandable_array<uint64_t> mateRecordIndex;
        expandable_array<uint64_t> mateAuId;
        expandable_array<expandable_array<expandable_array<uint64_t> > > reverseComp;
        expandable_array<uint64_t> numberOfSegmentMappings;
        uint64_t numberOfMappedRecordSegments;
        uint64_t readGroupId;
        bool has_readGroupId = false;
        expandable_array<expandable_array<std::string> > splicedSequence;
        int64_t pRef;
        uint64_t mappedLength;

        decodeNumberOfSegmentVariables(AU, collection, RR, previousMappingPos0, decoded_symbols,
                                       numberOfTemplateSegments, numberOfRecordSegments, numberOfAlignedRecordSegments,
                                       unpairedRead);
        decodeCLIPS(AU, collection, RR, previousMappingPos0, decoded_symbols, numberOfAlignedRecordSegments, softClips,
                    softClipSizes, hardClips, S, currentRecordCount);
        decodeRLEN(AU, collection, RR, previousMappingPos0, decoded_symbols, numberOfRecordSegments,
                   numberOfAlignedRecordSegments, hardClips, readLength, numberOfSplicedSeg, splicedSegLength);
        decodeMMAP(AU, collection, RR, previousMappingPos0, decoded_symbols, numberOfRecordSegments, unpairedRead,
                   numberOfAlignments, numberOfSegmentAlignments, numberOfAlignmentPairs, alignPtr, moreAlignments,
                   moreAlignmentsNextPos, moreAlignmentsNextSeqId);
        decodePOS(AU, collection, RR, previousMappingPos0, decoded_symbols, numberOfSegmentAlignments, mappingPos);
        decodePAIR(AU, collection, RR, previousMappingPos0, decoded_symbols, numberOfTemplateSegments, mappingPos,
                   read1First, delta, splitMate, splicedSegMappingPos, mateSeqId, mateRecordIndex, mateAuId);
        decodeRCOMP(AU, collection, RR, previousMappingPos0, decoded_symbols, mappingPos, numberOfRecordSegments,
                    numberOfAlignedRecordSegments, numberOfSegmentAlignments, splitMate, numberOfSplicedSeg,
                    reverseComp, numberOfSegmentMappings, numberOfMappedRecordSegments);
        decodeRGROUP(AU, collection, RR, previousMappingPos0, decoded_symbols, readGroupId, has_readGroupId);
        decodeSplicedSequence(AU, collection, RR, previousMappingPos0, decoded_symbols, numberOfMappedRecordSegments,
                              numberOfSplicedSeg, mappingPos, softClipSizes, splicedSegLength, splicedSequence, pRef,
                              mappedLength);

        reads_count -= numberOfRecordSegments;
    }
}

#endif