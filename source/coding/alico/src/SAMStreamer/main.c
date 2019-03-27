//
//  main.c
//  XC_s2fastqIO
//
//  Created by Mikel Hernaez on 11/4/14.
//  Copyright (c) 2014 Mikel Hernaez. All rights reserved.
//


#include <sys/wait.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include "sam_block.h"
#include "Arithmetic_stream.h"
#include "read_compression.h"
#include "edit.h"
#include "main.h"

#include <pthread.h>

static const char *MAPPED_READS = "mapped_reads";
static const char *HEADERS = "headers";
static const char *UNMAPPED_READS = "unmapped_reads";
static const char *ZIPPED_READS = "unmapped_reads.gz";
static const char *REFEREN_COMP = "reference_comp";
static const char *REFEREN_NUM = "reference_num";
static const char *REFEREN_LOCAL = "reference_local.fa";
static const char *QUAL_VALUES = "quality_values";
static const char *CALQ_QV = "quality_values_calq";

static const char *MOST_COMMON_LIST = "most_common_list";
static const char *SAME_REF = "same_ref";
static const char *MAPQ = "mapq";
static const char *RNEXT_SAME = "same";
static const char *POS = "pos";
static const char *VAR = "var";
static const char *FLAG = "flag";
static const char *SNPS = "snps";
static const char *INDELS = "indel";
static const char *CIGARFLAG = "cigarFlags";
static const char *CIGAR = "cigar";
static const char *FTLEN_ZERO = "zero";
static const char *FTLEN_SIGN = "sign";
static const char *PNEXT_ZERO = "zero1";
static const char *PNEXT_SIGN = "sign1";
static const char *MOST_COMMON_FLAG = "most_common_flag";
static const char *LUTFLAG = "lutf";
static const char *RAW = "raw";
static const char *LUT = "lut";
static const char *QV = "qv";

/**
 * Displays a usage name
 * @param name Program name string
 */
void usage(const char *name) {
    printf("Usage: %s (options) [input file] [output] [ref file]\n", name);
    printf("Options are:\n");
    //printf("\t-q\t\t\t: Store quality values in compressed file (default)\n");
    printf("\t-x\t\t: Regenerate file from compressed file\n");
    printf("\t-c [ratio]\t: Compress using [ratio] bits per bit of input entropy per symbol\n");
    printf("\t-d [rate]\t: Decompress and Download file from remote [output]\n");
    printf("\t-u [rate]\t: Compress and Upload file to remote [output]\n");
    printf("\t-s [rate]\t: Stream file to remote [output]\n");
    printf("\t-D [M|L|A]\t: Optimize for MSE, Log(1+L1), L1 distortions, respectively (default: MSE)\n");
    //printf("\t-c [#]\t\t: Compress using [#] clusters (default: 3)\n");
    //printf("\t-u [FILE]\t: Write the uncompressed lossy values to FILE (default: off)\n");
    printf("\t-q\t\t: CALQ Mode\n");
    printf("\t-z\t\t: CALQ Decompress Mode\n");
    //printf("\t-s\t\t: Print summary stats\n");
    //printf("\t-t [lines]\t: Number of lines to use as training set (0 for all, 1000000 default)\n");
    printf("\t-p [num]\t: Set the polyploidy value for CALQ (default: 2)\n");
    printf("\t-a [num]\t: Set the max quality value for CALQ (default: 41)\n");
    printf("\t-i [num]\t: Set the min quality value for CALQ (default: 0)\n");
    printf("\t-o [num]\t: Set the quality value offset for CALQ (default: 33)\n");
    //printf("\t-v\t\t: Enable verbose output\n");
    printf("\t-e\t\t: Compress reference file along with SAM file\n");
    printf("\t-m\t\t: Decompress reference file along with SAM file\n");
    printf("\t-h\t\t: Print this help\n");
}


void change_dir(char *dir) {
    if (chdir(dir) == -1) {
        fprintf(stderr, "Failed to change directories to %s\n", dir);
        return;
    }
}

void make_dir(char *dirname) {
    struct stat sb;
    if (!(stat(dirname, &sb) == 0 && S_ISDIR(sb.st_mode)) && mkdir(dirname, 0777) == -1) {
        fprintf(stderr, "Failed to create directory %s\n", dirname);
    }
}

void write_headers(FILE *input, FILE *output) {
    char c;
    while ( (c = getc(input)) != EOF) {
        if (c != '@') {
          break;
        }
        do {
            putc(c, output);
            c = getc(input);
        } while (c != '\n' && c != EOF);
        if (c == '\n') putc(c, output);
    }
    rewind(input);
}

void output_setter(void *thread_info, int flag) {
    char* mode;
    if(flag == 0) {
        mode = "wb";
    }
    else {
        mode = "rb";
    }

    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);

    memset(info->icodebook, 0, 1024*sizeof(int));

    info->fmost_common_list = fopen(MOST_COMMON_LIST, mode);
    info->fsame_ref = fopen(SAME_REF, mode);

    memset(info->irname, 0, 256*sizeof(int));
    memset(info->itoken_type, 0, 128*sizeof(int));
    memset(info->ialpha_len, 0, 128*sizeof(int));
    memset(info->ialpha_value, 0, 128*sizeof(int));
    memset(info->izero_run, 0, 128*sizeof(int));
    memset(info->idelta, 0, 128*sizeof(int));
    memset(info->iinteger, 0, 512*sizeof(int));
    memset(info->ichars, 0, 128*sizeof(int));

    info->fmapq = fopen(MAPQ, mode);
    info->rnext_same = fopen(RNEXT_SAME, mode);

    memset(info->rnext_prev, 0, 256*sizeof(int));
    memset(info->rlength, 0, 4*sizeof(int));

    info->fpos = fopen(POS, mode);
    info->fvar = fopen(VAR, mode);

    memset(info->rpos_alpha, 0, 4*sizeof(int));

    info->fflag = fopen(FLAG, mode);

    memset(info->rmatch, 0, 4*sizeof(int));

    info->fsnps = fopen(SNPS, mode);
    info->findels = fopen(INDELS, mode);

    memset(info->iread_chars, 0, 6*sizeof(int));

    info->fcigarFlags = fopen(CIGARFLAG, mode);
    info->cigar = fopen(CIGAR, mode);
    info->ftlen_zero = fopen(FTLEN_ZERO, mode);
    info->ftlen_sign = fopen(FTLEN_SIGN, mode);

    memset(info->itlen_tlen, 0, 4*sizeof(int));

    info->fpnext_zero = fopen(PNEXT_ZERO, mode);
    info->fpnext_sign = fopen(PNEXT_SIGN, mode);

    memset(info->idiff_pnext, 0, 4*sizeof(int));

    info->fmost_common_flag = fopen(MOST_COMMON_FLAG, mode);

    memset(info->imost_common_values_wContext, 0, 102*sizeof(int));
    memset(info->iaux_TagType, 0, 102*sizeof(int));
    memset(info->itag, 0, 2*sizeof(int));

    info->ftypeLUTflag = fopen(LUTFLAG, mode);
    info->ftypeRAW = fopen(RAW, mode);
    info->ftypeLUT = fopen(LUT, mode);

    memset(info->iintegers_wContext, 0, 408*sizeof(int));
    memset(info->idescBytes_wContext, 0, 204*sizeof(int));
    memset(info->iiidBytes_wContext, 0, 102*sizeof(int));

    info->fqv = fopen(QV, mode);

    return;
}

void file_closer(void *thread_info) {
    int i;
    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);

    for(i=0; i<1024; i++) {
      if(info->icodebook[i]>0) {fclose(info->fcodebook[i]);}
    }

    fclose(info->fmost_common_list);
    fclose(info->fsame_ref);

    for(i=0; i<256; i++) {
      if(info->irname[i]>0) {fclose(info->frname[i]);}
    }

    for(i=0; i<128; i++) {
      if(info->itoken_type[i]>0) {fclose(info->ftoken_type[i]);}
    }

    for(i=0; i<128; i++) {
      if(info->ialpha_len[i]>0) {fclose(info->falpha_len[i]);}
    }

    for(i=0; i<128; i++) {
      if(info->ialpha_value[i]>0) {fclose(info->falpha_value[i]);}
    }

    for(i=0; i<128; i++) {
      if(info->izero_run[i]>0) {fclose(info->fzero_run[i]);}
    }

    for(i=0; i<128; i++) {
      if(info->idelta[i]>0) {fclose(info->fdelta[i]);}
    }

    for(i=0; i<512; i++) {
      if(info->iinteger[i]>0) {fclose(info->finteger[i]);}
    }

    for(i=0; i<128; i++) {
      if(info->ichars[i]>0) {fclose(info->fchars[i]);}
    }

    fclose(info->fmapq);
    fclose(info->rnext_same);

    for(i=0; i<256; i++) {
      if(info->rnext_prev[i]>0) {fclose(info->frnext_prev[i]);}
    }
    for(i=0; i<4; i++) {
      if(info->rlength[i]>0) {fclose(info->frlength[i]);}
    }

    fclose(info->fpos);

    for(i=0; i<4; i++) {
      if(info->rpos_alpha[i]>0) {fclose(info->fpos_alpha[i]);}
    }

    fclose(info->fflag);

    for(i=0; i<4; i++) {
      if(info->rmatch[i]>0) {fclose(info->fmatch[i]);}
    }

    fclose(info->fsnps);
    fclose(info->findels);
    fclose(info->fvar);

    for(i=0; i<6; i++) {
      if(info->iread_chars[i]>0) {fclose(info->read_chars[i]);}
    }

    fclose(info->fcigarFlags);
    fclose(info->cigar);
    fclose(info->ftlen_zero);
    fclose(info->ftlen_sign);

    for(i=0; i<4; i++) {
      if(info->itlen_tlen[i]>0) {fclose(info->ftlen_tlen[i]);}
    }

    fclose(info->fpnext_zero);
    fclose(info->fpnext_sign);

    for(i=0; i<4; i++) {
      if(info->idiff_pnext[i]>0) {fclose(info->fdiff_pnext[i]);}
    }

    fclose(info->fmost_common_flag);

    for(i=0; i<102; i++) {
      if(info->imost_common_values_wContext[i]>0) {fclose(info->fmost_common_values_wContext[i]);}
    }

    for(i=0; i<102; i++) {
      if(info->iaux_TagType[i]>0) {fclose(info->faux_TagType[i]);}
    }

    for(i=0; i<2; i++) {
      if(info->itag[i]>0) {fclose(info->ftag[i]);}
    }

    fclose(info->ftypeLUTflag);
    fclose(info->ftypeRAW);
    fclose(info->ftypeLUT);

    for(i=0; i<408; i++) {
      if(info->iintegers_wContext[i]>0) {fclose(info->fintegers_wContext[i]);}
    }

    for(i=0; i<204; i++) {
      if(info->idescBytes_wContext[i]>0) {fclose(info->fdescBytes_wContext[i]);}
    }

    for(i=0; i<102; i++) {
      if(info->iiidBytes_wContext[i]>0) {fclose(info->fiidBytes_wContext[i]);}
    }

    fclose(info->fqv);

    return;
}

int alico_main(int argc, const char * argv[]) {

    int calq = 0;

    uint32_t mode, file_idx = 0, rc = 0, lossiness = LOSSLESS;
    int i = 0;

    struct qv_options_t opts;

    char input_name[1024], output_name[1024], ref_name[1024];

    char* ptr;

    struct remote_file_info remote_info;

    struct compressor_info_t comp_info;

    pthread_t compressor_thread;
    pthread_t network_thread;

    time_t begin_main;
    time_t end_main = 0;

    mkdir("/tmp/idoFiles", S_IRWXO | S_IRWXU | S_IRWXG);

    file_available = 0;

    opts.training_size = 1000000;
    opts.training_period = 0;
    opts.verbose = 0;
    opts.stats = 0;
    opts.ratio = 1;
    opts.uncompressed = 0;
    opts.distortion = DISTORTION_MSE;
    opts.polyploidy = 2;
    opts.qualityValueMax = 41;
    opts.qualityValueMin = 0;
    opts.qualityValueOffset = 33;
    mode = COMPRESSION;

    comp_info.compress_ref = 0;
    comp_info.decompress_ref = 0;
    // No dependency, cross-platform command line parsing means no getopt
    // So we need to settle for less than optimal flexibility (no combining short opts, maybe that will be added later)

    i = 1;
    while (i < argc) {
        // Handle file names and reject any other untagged arguments
        if (argv[i][0] != '-') {
            switch (file_idx) {
                case 0:
                    strcpy(input_name,argv[i]);
                    file_idx = 1;
                    printf("%s\n", argv[i]);
                    break;
                case 1:
                    strcpy(output_name,argv[i]);
                    file_idx = 2;
                    printf("%s\n", argv[i]);
                    break;
                case 2:
                    strcpy(ref_name, argv[i]);
                    file_idx = 3;
                    printf("%s\n", argv[i]);
                    break;
                default:
                    printf("Garbage argument \"%s\" detected.\n", argv[i]);
                    usage(argv[0]);
                    return 1;
            }
            i += 1;
            continue;
        }

        // Flags for options
        switch(argv[i][1]) {
            // COMPRESSION
            case 'c':
                mode = COMPRESSION;
                if ( (opts.ratio = atof(argv[i+1])) == 1) {
                    lossiness = LOSSLESS;
                }
                else
                    lossiness = LOSSY;
                opts.mode = MODE_RATIO;
                i += 2;
                break;
            // COMPRESSION WITH CALQ
            case 'q':
                mode = COMPRESSION;
                lossiness = LOSSY;
                opts.mode = MODE_RATIO;
                calq = 1;
                i += 1;
                break;
            // UPLOAD
            case 'u':
                mode = UPLOAD;
                if ( (opts.ratio = atof(argv[i+1])) == 1) {
                    lossiness = LOSSLESS;
                }
                else
                    lossiness = LOSSY;
                opts.mode = MODE_RATIO;
                i += 2;
                break;
            // EXTRACT
            case 'x':
                mode = DECOMPRESSION;
                i += 1;
                break;
            // DECOMPRESSION WITH CALQ
            case 'z':
                mode = DECOMPRESSION;
                calq = 1;
                i +=1;
                break;
            // REMOTE_DECOMPRESSION
            case 'r':
                mode = REMOTE_DECOMPRESSION;
                i += 1;
                break;
            // DOWNLOAD
            case 'd':
                mode = DOWNLOAD;
                i += 1;
                break;
            // VERBOSE
            case 'v':
                opts.verbose = 1;
                i += 1;
                break;
            // HELP
            case 'h':
                usage(argv[0]);
                exit(0);
            case 's':
                opts.stats = 1;
                i += 1;
                break;
            case 't':
                opts.training_size = atoi(argv[i+1]);
                i += 2;
                break;
            case 'w':
                opts.training_period = atoi(argv[i+1]);
                i += 2;
                break;
            // DISTORTION
            case 'D':
                switch (argv[i+1][0]) {
                    case 'M':
                        opts.distortion = DISTORTION_MSE;
                        break;
                    case 'L':
                        opts.distortion = DISTORTION_LORENTZ;
                        break;
                    case 'A':
                        opts.distortion = DISTORTION_MANHATTAN;
                        break;
                    default:
                        printf("Distortion measure not supported, using MSE.\n");
                        break;
                }
                i += 2;
                break;
            case 'p':
                opts.polyploidy = atoi(argv[i+1]);
                lossiness = LOSSY;
                opts.mode = MODE_RATIO;
                calq = 1;
                i += 2;
                break;
            case 'a':
                lossiness = LOSSY;
                opts.mode = MODE_RATIO;
                calq = 1;
                opts.qualityValueMax = atoi(argv[i+1]);
                i += 2;
                break;
            case 'i':
                lossiness = LOSSY;
                opts.mode = MODE_RATIO;
                calq = 1;
                opts.qualityValueMin = atoi(argv[i+1]);
                i += 2;
                break;
            case 'o':
                lossiness = LOSSY;
                opts.mode = MODE_RATIO;
                calq = 1;
                opts.qualityValueOffset = atoi(argv[i+1]);
                i += 2;
                break;
            case 'e':
                mode = COMPRESSION;
                comp_info.compress_ref = 1;
                i += 1;
                break;
            case 'm':
                mode = DECOMPRESSION;
                comp_info.decompress_ref = 1;
                i += 1;
                break;
            default:
                printf("Unrecognized option -%c.\n", argv[i][1]);
                usage(argv[0]);
                return 1;
        }
    }

    if (file_idx != 3 && mode == COMPRESSION) {
        printf("Missing required filenames.\n");
        usage(argv[0]);
        return 1;
    }
    if (file_idx != 2 && mode == DECOMPRESSION && comp_info.decompress_ref == 1){
        printf("Missing required filenames.\n");
        usage(argv[0]);
        return 1;
    }

    file_available = 0;

    time(&begin_main);

    comp_info.mode = mode;
    comp_info.lossiness = lossiness;

    if (calq == 1) {
        comp_info.calqmode = 1;
    } else {
        comp_info.calqmode = 0;
    }

    switch (mode) {
        case COMPRESSION: {
            comp_info.fsam = fopen( input_name, "r");
	          comp_info.fref = fopen( ref_name, "r"); //charlie

            if (calq) {
                if ( comp_info.fsam == NULL ) {
                    fputs ("File error while opening sam file\n", stderr);
                    return 1;
                }
            }
            else {
                if ( comp_info.fref == NULL || comp_info.fsam == NULL ){
                    fputs ("File error while opening ref and sam files\n",stderr);
                    return 1;
                }
            }

            make_dir(output_name);
            change_dir(output_name);

            FILE *headers_file = fopen(HEADERS, "w");
            write_headers(comp_info.fsam, headers_file);
            fclose(headers_file);

            comp_info.funmapped = fopen(UNMAPPED_READS, "w");

            comp_info.fcomp = fopen(MAPPED_READS, "w");
            output_setter((void *)&comp_info, 0);

            comp_info.frefcom = fopen(REFEREN_COMP, "w"); //hongyi
            comp_info.fsinchr = fopen(REFEREN_NUM, "w"); //hongyi
            comp_info.qv_opts = &opts;

            compress((void *)&comp_info);
            fclose(comp_info.funmapped);

            pid_t pid = fork();
            if (pid == 0) {
                char* argv[4];
                argv[0] = (char*)malloc(15*sizeof(char));
                argv[1] = (char*)malloc(15*sizeof(char));
                argv[2] = (char*)malloc(15*sizeof(char));
                strcpy(argv[0], "gzip");
                strcpy(argv[1], "-f");
                strcpy(argv[2], UNMAPPED_READS);
                argv[3] = NULL;
                execvp(argv[0], argv);
                free(argv[0]);free(argv[1]);free(argv[2]);
                return 1;
            }
            waitpid(pid, NULL, 0);
            fclose(comp_info.fsam);
            fclose(comp_info.fref);
            fclose(comp_info.fcomp);
            fclose(comp_info.frefcom);
            fclose(comp_info.fsinchr);
            file_closer((void *)&comp_info);

            free(reference);
            time(&end_main);
            change_dir("..");
            break;
                          }
        case DECOMPRESSION: {

            comp_info.fsam = fopen(output_name, "w");

            if (comp_info.decompress_ref) {
                change_dir(input_name);
                comp_info.fref = fopen(REFEREN_LOCAL, "w+");
                comp_info.frefcom = fopen(REFEREN_COMP, "r");
                comp_info.fsinchr = fopen(REFEREN_NUM, "r");
            }
            else {
                comp_info.fref = fopen(ref_name, "r");
                change_dir(input_name);
            }
            // CALQ IMPLEMENTATION
            //if (calq) {
                strcpy(comp_info.cq_name, CALQ_QV);
            //}

            if ( comp_info.fref == NULL || comp_info.fsam == NULL ){
                perror(output_name);
                fputs ("File error while opening ref and sam files\n",stderr);
                return 1;
            }

            if (comp_info.decompress_ref) {
                reconstruct_ref((void *)&comp_info);
                fseek(comp_info.fref, 0, SEEK_SET);
                fclose(comp_info.frefcom);
                fclose(comp_info.fsinchr);
            }

            pid_t pid = fork();
            if (pid == 0) {
                char* argv[4];
                argv[0] = (char*)malloc(18*sizeof(char));
                argv[1] = (char*)malloc(18*sizeof(char));
                argv[2] = (char*)malloc(18*sizeof(char));
                strcpy(argv[0], "gzip");
                strcpy(argv[1], "-df");
                strcpy(argv[2], ZIPPED_READS);
                argv[3] = NULL;
                execvp(argv[0], argv);
                free(argv[0]);free(argv[1]);free(argv[2]);
                return 1;
            }

            FILE *headers_file = fopen(HEADERS, "r");
            write_headers(headers_file, comp_info.fsam);
            fclose(headers_file);

            comp_info.fcomp = fopen(MAPPED_READS, "r");
            comp_info.fqual = fopen(QUAL_VALUES, "r");
            comp_info.qv_opts = &opts;
            output_setter((void *)&comp_info, 1);
            decompress((void *)&comp_info);

            waitpid(pid, NULL, 0);
            comp_info.funmapped = fopen(UNMAPPED_READS, "r");
            char c;
            while ( (c = getc(comp_info.funmapped)) != EOF) {
                putc(c, comp_info.fsam);
            }

            fclose(comp_info.fsam);
            fclose(comp_info.fref);
            fclose(comp_info.funmapped);
            fclose(comp_info.fcomp);
            file_closer((void *)&comp_info);
            time(&end_main);
            change_dir("..");
            break;
                            }
        case REMOTE_DECOMPRESSION:
            comp_info.fsam = fopen(output_name, "w");
            comp_info.fref = fopen ( ref_name , "r" );
            comp_info.fcomp = NULL;
            if ( comp_info.fref == NULL || comp_info.fsam == NULL ){
                fputs ("File error while opening ref and sam files\n",stderr); return 1;
            }
            comp_info.qv_opts = &opts;

            decompress((void *)&comp_info);
            time(&end_main);
            break;

        case UPLOAD:
            ptr = strtok((char*)output_name, "@");
            strcpy(remote_info.username, ptr);
            ptr = strtok(NULL, ":");
            strcpy(remote_info.host_name, ptr);
            ptr = strtok(NULL, "\0");
            strcpy(remote_info.filename, ptr);
            comp_info.fsam = fopen( input_name, "r");
            comp_info.fref = fopen ( ref_name , "r" );
            if ( comp_info.fref == NULL || comp_info.fsam == NULL ){
                fputs ("File error while opening ref and sam files\n",stderr); return 1;
            }
            comp_info.qv_opts = &opts;

            rc = pthread_create(&compressor_thread, NULL, compress , (void *)&comp_info);
            upload((void *)&remote_info);

            time(&end_main);
            pthread_exit(NULL);


            break;

        case DOWNLOAD:
            ptr = strtok((char*)input_name, "@");
            strcpy(remote_info.username, ptr);
            ptr = strtok(NULL, ":");
            strcpy(remote_info.host_name, ptr);
            ptr = strtok(NULL, "\0");
            strcpy(remote_info.filename, ptr);
            comp_info.fsam = fopen(output_name, "w");
            comp_info.fref = fopen ( ref_name , "r" );
            if ( comp_info.fref == NULL || comp_info.fsam == NULL ){
                fputs ("File error while opening ref and sam files\n",stderr); return 1;
            }
            comp_info.qv_opts = &opts;

            rc = pthread_create(&compressor_thread, NULL, download , (void *)&remote_info);
            decompress((void *)&comp_info);

            time(&end_main);
            pthread_exit(NULL);

            break;

        case STREAMING:

            comp_info.mode = UPLOAD;
            ptr = strtok((char*)output_name, "@");
            strcpy(remote_info.username, ptr);
            ptr = strtok(NULL, ":");
            strcpy(remote_info.host_name, ptr);
            ptr = strtok(NULL, "\0");
            strcpy(remote_info.filename, ptr);
            comp_info.fsam = fopen( input_name, "r");
            comp_info.fref = fopen ( ref_name , "r" );
            if ( comp_info.fref == NULL || comp_info.fsam == NULL ){
                fputs ("File error while opening ref and sam files\n",stderr); exit (1);
            }
            comp_info.qv_opts = &opts;

            rc = pthread_create(&network_thread, NULL, remote_decompression , (void *)&comp_info);
            rc = pthread_create(&compressor_thread, NULL, compress , (void *)&comp_info);
            upload((void *)&remote_info);

            time(&end_main);
            pthread_exit(NULL);


            break;


        default:
            break;
    }



    if (rc){
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }



    printf("Total time elapsed: %ld seconds.\n",(end_main - begin_main));

    return 0;




}
