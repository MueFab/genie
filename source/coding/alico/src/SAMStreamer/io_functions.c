//
//  io_functions.c
//  XC_s2fastqIO
//
//  Created by Mikel Hernaez on 12/16/14.
//  Copyright (c) 2014 Mikel Hernaez. All rights reserved.
//

#include "Arithmetic_stream.h"

int clean_compressed_dir(struct io_stream_t* ios){
    
    uint32_t rc=0;
    ios->fileCtr = 0;
    
    do{
        sprintf(ios->filePath, IDOFILE_PATH_ROOT "%010d", ios->fileCtr);
        rc = remove(ios->filePath);
        ios->fileCtr++;
    }while (rc == 0);
    
    ios->fileCtr = 0;
    
    return rc;
    
}

void open_new_iofile(struct io_stream_t* ios){
    
    switch (ios->mode) {
        case COMPRESSION:
            sprintf(ios->filePath, IDOFILE_PATH_ROOT "%010d", ios->fileCtr);
            ios->fileCtr++;
            
            ios->fp = fopen(ios->filePath, "w");
            break;
            
        case DECOMPRESSION:
            sprintf(ios->filePath, IDOFILE_PATH_ROOT "%010d", ios->fileCtr);
            ios->fileCtr++;
            
            ios->fp = fopen(ios->filePath, "r");
            fread(ios->buf, sizeof(uint8_t), IO_STREAM_BUF_LEN, ios->fp);
            fclose(ios->fp);
//MODIFY            remove(ios->filePath);

            break;
            
        case UPLOAD:
            sprintf(ios->filePath, IDOFILE_PATH_ROOT "%010d", ios->fileCtr);
            ios->fileCtr++;
            
            ios->fp = fopen(ios->filePath, "w");
            break;
            
        case DOWNLOAD:
            sprintf(ios->filePath, IDOFILE_PATH_ROOT "%010d", ios->fileCtr);
            ios->fileCtr++;
            
            while (file_available == 0) ;
            ios->fp = fopen(ios->filePath, "r");
            fread(ios->buf, sizeof(uint8_t), IO_STREAM_BUF_LEN, ios->fp);
            fclose(ios->fp);
            //        remove(ios->filePath);
            file_available--;
            break;
        default:
            break;
    }
    /*if (ios->mode == DECOMPRESSION) {
        while (file_available == 0) ;
        ios->fp = fopen(ios->filePath, "r");
        fread(ios->buf, sizeof(uint8_t), IO_STREAM_BUF_LEN, ios->fp);
        fclose(ios->fp);
//        remove(ios->filePath);
        file_available--;
        
    }
    else ios->fp = fopen(ios->filePath, "w");*/
}

/**
 * Writes out the current stream buffer regardless of fill amount
 */
void stream_write_buffer(struct io_stream_t *os) {
    
    FILE *temp_lock;
    char lockname[256];
    
    int rc;
    
    switch (os->mode) {
        case COMPRESSION:
            fwrite(os->buf, sizeof(uint8_t), os->bufPos, os->fp);
            memset(os->buf, 0, sizeof(uint8_t)*(os->bufPos));
            os->written += os->bufPos;
            os->bufPos = 0;
            break;
            
        case DECOMPRESSION:
            fread(os->buf, sizeof(uint8_t), IO_STREAM_BUF_LEN, os->fp);
            os->bufPos = 0;
            break;
            
        case UPLOAD:
            sprintf(os->filePath, IDOFILE_PATH_ROOT "%010d", os->fileCtr);
            os->fileCtr++;
            
            //define and create the "ready" file
            strcpy(lockname, os->filePath);
            strcat(lockname, "_ready");
            //
            
            os->fp = fopen(os->filePath, "w");
            fwrite(os->buf, sizeof(uint8_t), os->bufPos, os->fp);
            fclose(os->fp);
            
            // create the "ready" file
            temp_lock = fopen(lockname, "w");
            fclose(temp_lock);
            //
            
            file_available++;
            memset(os->buf, 0, sizeof(uint8_t)*(os->bufPos));
            os->written += os->bufPos;
            os->bufPos = 0;
            break;
            
        case DOWNLOAD:
            sprintf(os->filePath, IDOFILE_PATH_ROOT "%010d", os->fileCtr);
            os->fileCtr++;
            
            while (file_available == 0) ;
            os->fp = fopen(os->filePath, "r");
            fread(os->buf, sizeof(uint8_t), IO_STREAM_BUF_LEN, os->fp);
            fclose(os->fp);
            remove(os->filePath);
            file_available--;
            
            os->bufPos = 0;
            
            break;
            
        case REMOTE_DECOMPRESSION:
            sprintf(os->filePath, IDOFILE_PATH_ROOT "%010d", os->fileCtr);
            os->fileCtr++;
            
            //define the "ready" file
            strcpy(lockname, os->filePath);
            strcat(lockname, "_ready");
            //
            
            while ( (rc = access( lockname, F_OK )) == -1) ;
            remove(lockname);
            
            os->fp = fopen(os->filePath, "r");
            fread(os->buf, sizeof(uint8_t), IO_STREAM_BUF_LEN, os->fp);
            fclose(os->fp);
            remove(os->filePath);
            
            os->bufPos = 0;
            
            break;

        default:
            break;
    }
}

/**
 * Fills out the current stream buffer regardless of fill amount
 */
void stream_fill_buffer(struct io_stream_t *is) {
    stream_write_buffer(is);
}