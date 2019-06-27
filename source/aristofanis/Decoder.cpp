#include "DataStructures.h"

using namespace helper_functions;
using namespace data_structures;

/* hacky i know */
/* probably bad programming practise too */
/* TODO make code less hacky */

#define subsequence0(ID, POS)   decoded_symbols[ID][0][AU.j[ID][POS]]
#define subsequence1(ID, POS)   decoded_symbols[ID][1][AU.j[ID][POS]]
#define subsequence2(ID, POS)   decoded_symbols[ID][2][AU.j[ID][POS]]
#define subsequence3(ID, POS)   decoded_symbols[ID][3][AU.j[ID][POS]]
#define subsequence4(ID, POS)   decoded_symbols[ID][4][AU.j[ID][POS]]
#define subsequence5(ID, POS)   decoded_symbols[ID][5][AU.j[ID][POS]]
#define subsequence6(ID, POS)   decoded_symbols[ID][6][AU.j[ID][POS]]
#define subsequence7(ID, POS)   decoded_symbols[ID][7][AU.j[ID][POS]]
#define subsequence8(ID, POS)   decoded_symbols[ID][8][AU.j[ID][POS]]
#define subsequence9(ID, POS)   decoded_symbols[ID][9][AU.j[ID][POS]]

#define subsequence0(ID)    decoded_symbols[ID][0]
#define subsequence1(ID)    decoded_symbols[ID][1]
#define subsequence2(ID)    decoded_symbols[ID][2]
#define subsequence3(ID)    decoded_symbols[ID][3]
#define subsequence4(ID)    decoded_symbols[ID][4]
#define subsequence5(ID)    decoded_symbols[ID][5]
#define subsequence6(ID)    decoded_symbols[ID][6]
#define subsequence7(ID)    decoded_symbols[ID][7]
#define subsequence8(ID)    decoded_symbols[ID][8]
#define subsequence9(ID)    decoded_symbols[ID][9]

const uint64_t minus1 = -1; // cause we are using unsigned types, pls use for equality testing

namespace decoder {

    void decode(access_unit AU, parameter_set PS, uint64_t &previousMappingPos0) {
        uint64_t currentRecordCount = 0;

        uint8_t classId                 = AU.header.AU_type; // 1
        uint64_t AU_start_position      = AU.header.AU_start_position;
        uint16_t seqId                  = AU.header.sequence_ID;

        uint8_t numberOfTemplateSegments    = PS.enc_params.number_of_template_segments_minus1+1;
        uint8_t alphabet_ID                 = PS.enc_params.alphabet_ID;
        uint32_t read_length                = PS.enc_params.read_length;
        bool spliced_reads_flag             = PS.enc_params.spliced_reads_flag;
        bool multiple_alignments_flag       = PS.enc_params.multiple_alignments_flag;

        uint8_t numberOfRecordSegments;
        uint8_t numberOfAlignedRecordSegments;
        uint8_t unpairedRead;

        // code that creates expandable_array onject decoded_symbols goes here

        if(numberOfTemplateSegments == 1)   numberOfRecordSegments = 1;
        else if(classId == CLASS_HM)        numberOfRecordSegments = 2;
        else if(subsequence0(8, 0) == 0)    numberOfRecordSegments = 2;
        else                                numberOfRecordSegments = 1; // 2.1

        if(classId == CLASS_HM)         numberOfAlignedRecordSegments = 1;
        else if(classId == CLASS_U)     numberOfAlignedRecordSegments = 0;
        else                            numberOfAlignedRecordSegments = numberOfRecordSegments; // 2.2
        
        if(classId == CLASS_HM)     unpairedRead = 0;
        else if(numberOfTemplateSegments == 1 || subsequence0(8, 0) == 5 || subsequence0(8, 0) == 6)
                                    unpairedRead = 1;
        else                        unpairedRead = 0; // 2.3

        expandable_array<expandable_array<std::string> >    softClips;
        expandable_array<expandable_array<uint64_t> >       softClipSizes;
        expandable_array<expandable_array<uint64_t> >       hardClips;
        expandable_array<std::string>                       S;

        initialize_supported_symbols(S);

        for(uint8_t i=0; i<numberOfAlignedRecordSegments; ++i) {
            for(uint8_t j=0; j<2; ++j) {
                softClips[i][j]     = "";
                softClipSizes[i][j] = 0;
                hardClips[i][j]     = 0;
            }
        }
        
        if(classId == CLASS_I || classId == CLASS_HM) {
            if(AU.j[5][0] < subsequence0(5).size() && currentRecordCount == subsequence0(5, 0)) {
                bool end = 0;

                uint64_t j;
                uint64_t segmentIdx;
                uint64_t leftRightIdx;

                do {
                    if(subsequence1(5, 1) <= 3) {
                        j               = 0;
                        segmentIdx      = subsequence1(5, 1) >> 1;
                        leftRightIdx    = subsequence1(5, 1) & 1;
                        
                        do {
                            softClips[segmentIdx][leftRightIdx][j] = S[alphabet_ID][subsequence2(5 ,2)];
                            AU.j[5][2]++;
                            j++;
                        } while(subsequence2(5, 2) != S[alphabet_ID].size())
                        
                        AU.j[5][2]++;
                        softClipSizes[segmentIdx][leftRightIdx] = j;
                    } 
                    else if(subsequence1(5, 1) <= 7) {
                        segmentIdx                          = (subsequence1(5, 1) - 4) >> 1;
                        leftRightIdx                        = (subsequence1(5, 1) - 5) & 1;
                        hardClips[segmentIdx][leftRightIdx] = subsequence3(5, 3);
                        AU.j[5][3]++;
                    } 
                    else if(subsequence1(5, 1) == 8)
                        end = 1;

                    AU.j[5][1]++;
                } while(!end);

                AU.j[5][0]++;
            }

            currentRecordCount++;
        } // 3

        expandable_array<uint64_t>                      readLength;
        expandable_array<uint64_t>                      numberOfSplicedSeg;
        expandable_array<expandable_array<uint64_t> >   splicedSegLength;

        if(read_length == 0) {
            for(uint8_t i=0; i<numberOfRecordSegments; ++i) {
                readLength[i] = subsequence0(7, 0) + 1;
                AU.j[7][0]++;
            }
        } 
        else {
            for(uint8_t i=0; i<numberOfRecordSegments; ++i) {
                if(classId == CLASS_I)                  readLength[i] = read_length - hardClips[i][0] - hardClips[i][1];
                else if(classId == CLASS_HM && i == 0)  readLength[i] = read_length - hardClips[0][0] - hardClips[0][1];
                else                                    readLength[i] = read_length;
            }
        }

        for(uint8_t i=0; i<numberOfRecordSegments; ++i) {
            numberOfSplicedSeg[i]   = 1;
            splicedSegLength[i][0]  = readLength[i];
        }

        if(spliced_reads_flag && (classId == CLASS_I || classId == CLASS_HM)) {
            uint64_t remainingLen;
            uint64_t spliceLen;
            uint64_t j;

            for(uint8_t i=0; i<numberOfAlignedRecordSegments; ++i) {
                if(read_length == 0) {
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
                } while(remainingLen > 0);

                numberOfSplicedSeg[i] = j;
            }
        } // 4

        uint64_t                                        numberOfAlignments;
        expandable_array<uint64_t>                      numberOfSegmentAlignments;
        expandable_array<uint64_t>                      numberOfAlignmentPairs;
        expandable_array<expandable_array<uint64_t> >   alignPtr;
        uint64_t                                        moreAlignments;
        uint64_t                                        moreAlignmentsNextPos;
        uint64_t                                        moreAlignmentsNextSeqId;

        if(classId != CLASS_U) {
            if(multiple_alignments_flag == 0)   
                numberOfSegmentAlignments[0] = 1;
            else {
                numberOfSegmentAlignments[0] = subsequence0(10, 0);
                AU.j[10][0]++;
            }
        }
        else numberOfSegmentAlignments[0] = 0;

        moreAlignments = 0;

        if(unpairedRead || classId == CLASS_HM) {
            numberOfAlignments = numberOfSegmentAlignments[0];

            for(uint64_t i=0; i<numberOfAlignments; ++i)
                alignPtr[i][0] = i;
        }
        else if(classId == CLASS_U) {
            if(numberOfRecordSegments > 1)
                numberOfSegmentAlignments[1] = 0;
            numberOfAlignments = 0;
        }
        else {
            numberOfSegmentAlignments[1] = 0;
            uint64_t k, i, j, ptr;
            k = i = 0;

            while(i < numberOfSegmentAlignments[0]) {
                if(multiple_alignments_flag == 0)   
                    numberOfAlignmentPairs[i] = 1;
                else {
                    numberOfAlignmentPairs[i] = subsequence0(10, 0);
                    AU.j[10][0]++;
                }

                j = 0;

                while(j < numberOfAlignmentPairs[i]) {
                    if(i != 0) {
                        ptr = subsequence1(10, 1);
                        AU.j[10][1]++;
                    }
                    else ptr = 0;

                    alignPtr[k][1] = numberOfSegmentAlignments[1] - ptr;
                    alignPtr[k][0] = i;

                    if(ptr == 0) numberOfSegmentAlignments[1]++;

                    j++;
                    k++;
                }

                i++;
            }

            numberOfAlignments = k;
        }

        if(multiple_alignments_flag == 1 && classId != CLASS_U && subsequence2(10, 2)) {
            moreAlignments = 1;
            moreAlignmentsNextSeqId = subsequence3(10, 3);
            moreAlignmentsNextPos   = subsequence4(10, 4);
            AU.j[10][3]++;
            AU.j[10][4]++;
        }

        AU.j[10][2]++; // 5

        expandable_array<expandable_array<uint64_t> > mappingPos;

        if(AU.j[0][0] > 0)  mappingPos[0][0] = previousMappingPos0 + subsequence0(0, 0);
        else                mappingPos[0][0] = AU_start_position + subsequence0(0, 0);

        previousMappingPos0 = mappingPos[0][0];

        for(uint64_t i=1; i<numberOfSegmentAlignments[0]; ++i) {
            mappingPos[i][0] = mappingPos[i-1][0] + subsequence1(0, 1);
            AU.j[0][1]++;
        }

        AU.j[0][0]++; // 6

        uint8_t read1First, delta;
        expandable_array<expandable_array<uint64_t> >   splitMate;
        expandable_array<expandable_array<uint64_t> >   splicedSegMappingPos;
        expandable_array<expandable_array<uint64_t> >   mateSeqId;
        expandable_array<uint64_t>                      mateRecordIndex;
        expandable_array<uint64_t>                      mateAuId;

        splitMate[0][0] = 0;

        if(classId != CLASS_HM) {
            for(uint64_t i=1; i<numberOfTemplateSegments; ++i) {
                if(subsequence0(8, 0) == 0) {
                    splitMate[0][i] = 0;
                    if(classId != CLASS_U) {
                        read1First  = (subsequence1(8, 1) & 0x0001) ? 0 : 1;
                        delta       = subsequence1(8, 1) >> 1;
                        mappingPos[0][i] = mappingPos[0][0] + delta;
                        mateSeqId[0][i]  = seqId;
                        AU.j[8][1]++;
                    }
                    else {
                        read1First          = 1;
                        mateAuId[1]         = minus1;
                        mateRecordIndex[i]  = minus1;
                    }
                }
                else if(subsequence0(8, 0) == 1) {
                    splitMate[0][i] = 1;
                    read1First      = 0;
                    if(classId != CLASS_U) {
                        mappingPos[0][i]    = subsequence2(8, 2);
                        mateSeqId[0][i]     = seqId;
                    }
                    else {
                        mateAuId[1]         = minus1;
                        mateRecordIndex[i]  = subsequence2(8, 2);
                    }
                    AU.j[8][2]++;
                }
                else if(subsequence0(8, 0) == 2) {
                    splitMate[0][i] = 1;
                    read1First      = 1;
                    if(classId != CLASS_U) {
                        mappingPos[0][i]    = subsequence3(8, 3);
                        mateSeqId[0][i]     = seqId;
                    } else {
                        mateAuId[1] = minus1;
                        mateRecordIndex[i] = subsequence2(8, 3);
                    }
                    AU.j[8][3]++;
                }
                else if(subsequence0(8, 0) == 3) {
                    splitMate[0][i] = 1;
                    read1First      = 0;
                    if(classId != CLASS_U) {
                        mateSeqId[0][i]     = subsequence4(8, 4);
                        mappingPos[0][i]    = subsequence6(8, 6);
                    }
                }
            }
        }
        
    }

} /* << end namespace decoder */

