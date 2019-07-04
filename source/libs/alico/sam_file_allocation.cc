//
//  sam_file_allocation.c
//  XC_s2fastqIO
//
//  Created by Mikel Hernaez on 11/18/14.
//  Copyright (c) 2014 Mikel Hernaez. All rights reserved.
//

#include "sam_block.h"



void reset_QV_block(qv_block qvb, uint8_t direction){

    //free_stream_model_qv(qvb->qlist, qvb->model);
    free_cond_quantizer_list(qvb->qlist);
    if (direction == COMPRESSION)
        free_conditional_pmf_list(qvb->training_stats);

}


// devuelve la longitud del PRIMER read?
// se asume que todos lso reads son de la misma longitud.

uint32_t get_read_length(FILE *f){

    int ch, header_bytes = 0;
    char buffer[10000]; // 2 KB buffer

    uint8_t keep = 1;
    int i, j;

    // We use this opportunity to remove the headers
    // getc coge caracter, fgets coge 2048 o hasta salto linea.
    // con esto simplemente calculamos los bytes de header (empiezan con @),
    // el contenido de buffer nos da igual (se override cada vez).
    while ((ch = getc(f)) == '@') {
        fgets(buffer, 10000, f);
        header_bytes += strlen(buffer) + 1; // +1 to account for the @
    }

    // rewind the file pointer to be at the beginning of the first read
    fseek(f, header_bytes, SEEK_SET);

    while (keep){
        // Extract the first read
        keep = 0;
        // first extract the cigar to check it doesn't have Hard clips (H)
        fscanf(f, "%*s %*d %*s %*d %*d %s", buffer);
        for (i=0;i<strlen(buffer);i++){
            if (!strncmp(&buffer[i],"H",1)){
                keep = 1;
                fgets(buffer, 10000, f); // finish reading line
                break;
            }
        }
    }
    // Now we are in a read with no HARD clips, so we get the read and its length
    fscanf(f, "%*s %*d %*d %s", buffer);
    //fscanf(f, "%*s %*d %*s %*d %*d %*s %*s %*d %*d %s", buffer);

    // rewind the file pointer to be at the beginning of the first read
    fseek(f, header_bytes, SEEK_SET);
    //printf("%d\n",strlen(buffer));

    return (uint32_t)strlen(buffer);

}


/**
 *
 */
rname_block alloc_rname_block(){
    uint32_t i = 0;

    rname_block rtn = (rname_block) calloc(1, sizeof(struct rname_block_t));

    rtn->block_length = MAX_LINES_PER_BLOCK;

    rtn->rnames = (char**) calloc(rtn->block_length, sizeof(char*));

    // allocate the memory for each of the lines
    for (i = 0; i < rtn->block_length; i++) {

        rtn->rnames[i] = (char*) calloc(MAX_READ_LENGTH, sizeof(char));
    }

    // Allocate (and initialize) the models for the rnames
    rtn->models = alloc_rname_models_t();

    return rtn;
}

/**
 *
 */
mapq_block alloc_mapq_block(){

    mapq_block rtn = (mapq_block) calloc(1, sizeof(struct mapq_block_t));

    rtn->block_length = 1;

    rtn->mapq = (char*) calloc(rtn->block_length, sizeof(char));

    // Allocate (and initialize) the models for the rnames
    rtn->models = alloc_mapq_models_t();

    return rtn;
}



/**
 *
 */
rnext_block alloc_rnext_block(){
    uint32_t i = 0;

    rnext_block rtn = (rnext_block) calloc(1, sizeof(struct rnext_block_t));

    rtn->block_length = 1;

    rtn->rnext = (char**) calloc(rtn->block_length, sizeof(char*));

    // allocate the memory for each of the lines
    for (i = 0; i < rtn->block_length; i++) {

        rtn->rnext[i] = (char*) calloc(MAX_READ_LENGTH, sizeof(char));
    }

    // Allocate (and initialize) the models for the rnames
    rtn->models = alloc_rnext_models_t();

    return rtn;
}

/**
 *
 */
pnext_block alloc_pnext_block(){

    pnext_block rtn = (pnext_block) calloc(1, sizeof(struct pnext_block_t));

    rtn->block_length = 1;

    rtn->pnext = (uint32_t *) calloc(rtn->block_length, sizeof(uint32_t*));

    // Allocate (and initialize) the models for the rnames
    rtn->models = alloc_pnext_models_t();

    return rtn;
}

/**
 *
 */
tlen_block alloc_tlen_block(){

    tlen_block rtn = (tlen_block) calloc(1, sizeof(struct tlen_block_t));

    rtn->block_length = 1;

    rtn->tlen = (int32_t *) calloc(rtn->block_length, sizeof(int32_t*));

    // Allocate (and initialize) the models for the rnames
    rtn->models = alloc_tlen_models_t();

    return rtn;
}



/**
 *
 */
id_block alloc_id_block(){

    uint32_t i = 0;

    id_block rtn = (id_block) calloc(1, sizeof(struct id_block_t));

    rtn->block_length = 1;

    rtn->IDs = (char**) calloc(rtn->block_length, sizeof(char*));

    // allocate the memory for each of the lines
    for (i = 0; i < rtn->block_length; i++) {

        rtn->IDs[i] = (char*) calloc(MAX_READ_LENGTH, sizeof(char));
    }

    // Allocate (and initialize) the models for the IDs
    rtn->models = alloc_id_models_t();

    return rtn;
}

/**
 *
 */
aux_block alloc_aux_block() {

    uint32_t i = 0;

    aux_block rtn = (aux_block) calloc(1, sizeof(struct aux_block_t));

    rtn->block_length = 10;

    rtn->aux_str = (char**) calloc(MAX_AUX_FIELDS, sizeof(char*));

    // allocate the memory for each of the lines
    for (i = 0; i < MAX_AUX_FIELDS; i++) {
        rtn->aux_str[i] = (char*) calloc(MAX_AUX_LENGTH, sizeof(char));
    }

    rtn->most_common = (char**) calloc(MOST_COMMON_LIST_SIZE, sizeof(char*));
    // allocate the memory for each of the lines
    for (i = 0; i < MOST_COMMON_LIST_SIZE; i++) {
        rtn->most_common[i] = (char*) calloc(MAX_AUX_LENGTH, sizeof(char));
    }

    // Allocate (and initialize) the models for the aux
    rtn->models = alloc_aux_models_t();

    return rtn;
}

/**
 *
 */
read_block alloc_read_block_t(uint32_t read_length){

    read_block rf = (read_block) calloc(1, sizeof(struct read_block_t));

    rf->block_length = 1;

    rf->lines = (read_line) calloc(rf->block_length, sizeof(struct read_line_t));

    rf->lines->cigar = (char*) calloc(1, 2*read_length);
    rf->lines->edits = (char*) calloc(1, 2*read_length);
    rf->lines->read = (char*) calloc(1, read_length + 3);
   
    // Allocate (and initialize) the models for the reads
    rf->models = alloc_read_models_t(read_length);
   
    return rf;
}

/**
 *
 */
qv_block alloc_qv_block_t(struct qv_options_t *opts, uint32_t read_length){

    qv_block qv_info;

    symbol_t *sym_buffer;

    uint32_t i = 0;

    uint32_t rescale = 1 << 20;

    qv_info = (qv_block) calloc(1, sizeof(struct qv_block_t));

    qv_info->block_length = opts->training_size;

    qv_info->columns = read_length;

    // Allocate the QV alphabet and the distortion matrix
    struct distortion_t *dist = generate_distortion_matrix(QV_ALPHABET_SIZE, opts->distortion);
    struct alphabet_t *alphabet = alloc_alphabet(QV_ALPHABET_SIZE);

    sym_buffer = (symbol_t*) calloc(qv_info->block_length, qv_info->columns*sizeof(symbol_t));

    qv_info->qv_lines = (struct qv_line_t *) calloc(qv_info->block_length, sizeof(struct qv_line_t));

    // allocate the memory for each of the lines
    for (i = 0; i < qv_info->block_length; i++) {

        //qv_info->qv_lines[i].data = (symbol_t*) calloc(qv_info->columns, sizeof(symbol_t));
        qv_info->qv_lines[i].data = sym_buffer;
        qv_info->qv_lines[i].columns = read_length;
        sym_buffer += read_length;
    }

    // set the alphabet and distortion
    qv_info->alphabet = alphabet;
    qv_info->dist = dist;

    qv_info->opts = opts;

    qv_info->model = alloc_stream_model_qv(read_length, QV_ALPHABET_SIZE + 1, rescale);

    return qv_info;

}

/**
 *
 */
simplified_qv_block alloc_simplified_qv_block_t(struct qv_options_t *opts, uint32_t read_length){

    simplified_qv_block qv_info;

    symbol_t *sym_buffer;

    uint32_t i = 0, input_alphabet_size = QV_ALPHABET_SIZE;

    uint32_t rescale = 1 << 20;

    qv_info = (simplified_qv_block) calloc(1, sizeof(struct qv_block_t));

    qv_info->block_length = MAX_LINES_PER_BLOCK;

    qv_info->columns = read_length;

    // Allocate the QV alphabet and the distortion matrix
    sym_buffer = (symbol_t*) calloc(qv_info->block_length, qv_info->columns*sizeof(symbol_t));

    qv_info->qv_lines = (struct qv_line_t *) calloc(qv_info->block_length, sizeof(struct qv_line_t));

    // allocate the memory for each of the lines
    for (i = 0; i < qv_info->block_length; i++) {

        //qv_info->qv_lines[i].data = (symbol_t*) calloc(qv_info->columns, sizeof(symbol_t));
        qv_info->qv_lines[i].data = sym_buffer;
        qv_info->qv_lines[i].columns = read_length;
        sym_buffer += read_length;
    }

    qv_info->model = alloc_stream_model_qv(read_length, input_alphabet_size, rescale);

    return qv_info;

}


sam_block alloc_sam_models(Arithmetic_stream as, FILE * fin, FILE *fref, struct qv_options_t *qv_opts, uint8_t mode, void *thread_info){
    struct compressor_info_t info = *((struct compressor_info_t *)thread_info);
    uint32_t i = 0;

    sam_block sb = (sam_block) calloc(1, sizeof(struct sam_block_t));

    sb->fs = fin;

    sb->fref = fref;

    // initialize the codebook_model
    uint32_t rescale = 1 << 20;
    sb->codebook_model = initialize_stream_model_codebook(rescale);

    // Get the Read Length
    if (mode == DECOMPRESSION || mode == DOWNLOAD) {
        // get the readLength from the ios buffer
        sb->read_length =  decompress_int(as, sb->codebook_model, thread_info);
        //printf("%d\n",sb->read_length);
    }
    else{
        // get the read length from input file and move file pointer after headers
        sb->read_length = get_read_length(sb->fs);
        printf("%d\n",sb->read_length);
        // write readLength directly to AS using the codebook model
        compress_int(as, sb->codebook_model, sb->read_length, thread_info);
    }
   

    // Allocate the memory for the three parts:

    //READS,
    sb->reads = alloc_read_block_t(sb->read_length);
  
    //QVs,
    sb->QVs = alloc_qv_block_t(qv_opts, sb->read_length);
    sb->QVs->codebook_model = sb->codebook_model;
    //WELL random generator
    memset(&(sb->QVs->well), 0, sizeof(struct well_state_t));
    if (mode == DECOMPRESSION || mode == DOWNLOAD) {
        for (i = 0; i < 32; ++i) {
            sb->QVs->well.state[i] = decompress_int(as, sb->codebook_model, thread_info);
        }
    }
    else{
        // Initialize WELL state vector with libc rand
        srand((uint32_t) time(0));
        for (i = 0; i < 32; ++i) {
#ifndef DEBUG
            sb->QVs->well.state[i] = rand();
            // Write the initial WELL state vector to the file first (fixed size of 32 bytes)
            compress_int(as, sb->codebook_model, sb->QVs->well.state[i], thread_info); //printf("once\n");printf("%d\n",info.icodebook[0]);
#else
            sb->QVs->well.state[i] = 0x55555555;
            // Write the initial WELL state vector to the file first (fixed size of 32 bytes)
            compress_int(as, sb->codebook_model, sb->QVs->well.state[i], thread_info);printf("twice\n");
#endif
        }
    }
    // Must start at zero
    sb->QVs->well.n = 0;

    sb->QVs->fs = sb->fs;

    //IDs
    sb->IDs = alloc_id_block();

    //aux
    sb->aux = alloc_aux_block();

    //RNAMEs
    sb->rnames = alloc_rname_block();

    //MAPQ
    sb->mapq = alloc_mapq_block();

    //RNEXT
    sb->rnext = alloc_rnext_block();

    //PNEXT
    sb->pnext = alloc_pnext_block();

    //TLEN
    sb->tlen = alloc_tlen_block();



    return sb;

}


uint32_t load_sam_block(sam_block sb){ //NOT IN USE! load_sam_line.

    int32_t i = 0, j = 0;
    read_line rline = NULL;
    qv_line qvline = NULL;


    char buffer[1024];
    char *ptr;
    char *ID_line;
    char *rname_line;

    for (i = 0; i < sb->block_length; i++) {

        rline = &(sb->reads->lines[i]);
        qvline = &(sb->QVs->qv_lines[i]);
        ID_line = sb->IDs->IDs[i];
        rname_line = sb->rnames->rnames[i];

        rline->read_length = sb->read_length;
        qvline->columns = sb->read_length;
        // Read compulsory fields
        if (fgets(buffer, 1024, sb->fs)) {
            // ID
            ptr = strtok(buffer, "\t");
            strcpy(ID_line, ptr);
            // FLAG
            ptr = strtok(NULL, "\t");
            rline->invFlag = atoi(ptr);
            // RNAME
            ptr = strtok(NULL, "\t");
            strcpy(rname_line, ptr);
            // POS
            ptr = strtok(NULL, "\t");
            rline->pos = atoi(ptr);
            // MAPQ
            ptr = strtok(NULL, "\t");
            // CIGAR
            ptr = strtok(NULL, "\t");
            strcpy(rline->cigar, ptr);
            // RNEXT
            ptr = strtok(NULL, "\t");
            // PNEXT
            ptr = strtok(NULL, "\t");
            // TLEN
            ptr = strtok(NULL, "\t");
            // SEQ
            ptr = strtok(NULL, "\t");
            strcpy(rline->read, ptr);
            // QUAL
            qvline->columns = (int)strlen(ptr);
            printf("Num cols is:%d\n",qvline->columns);
            ptr = strtok(NULL, "\t");
            // Read the QVs and translate them to a 0-based scale
            // Check if the read is inversed
            if (rline->invFlag & 16) { // The read is inverted
                for (j = qvline->columns - 1; j >= 0; j--) {
                    qvline->data[j] = (*ptr) - 33, ptr++;
                }
            }
            else{ // The read is not inversed
                for (j = 0; j < qvline->columns; j++) {
                    qvline->data[j] = (*ptr) - 33, ptr++;
                }
            }
            // Read the AUX fields until end of line, and store the MD field
            while( NULL != (ptr = strtok(NULL, "\t")) ){
                // Do something
                if (*ptr == 'M' && *(ptr+1) == 'D'){
                    // skip MD:Z:
                    ptr += 5;
                    strcpy(rline->edits, ptr);
                    break;
                }

            }
        }
        else
            break;
       /* int ch = 0;
        if (EOF!=fscanf(sb->fs, "%*s %"SCNu16" %*s %d %*d %s %*s %*d %*d %s", &(rline->invFlag), &(rline->pos), rline->cigar, rline->read)){
          fgetc(sb->fs); //remove the \t

            // Read the QVs and translate them to a 0-based scale
            // Check if the read is inversed
            if (rline->invFlag & 16) { // The read is inversed
                for (j = sb->read_length - 1; j >= 0; j--) {
                    qvline->data[j] = fgetc(sb->fs) - 33;
                }
            }
            else{ // The read is not inversed
                for (j = 0; j < sb->read_length; j++) {
                    qvline->data[j] = fgetc(sb->fs) - 33;
                }
            }


            // Read the AUX fields until end of line, and store the MD field
            while('\n'!=(ch=fgetc(sb->fs))){
                // Do something
                if (ch == 'M'){
                    ch = fgetc(sb->fs);
                    if (ch == 'D'){
                        // Read :Z:
                        fgetc(sb->fs);// :
                        fgetc(sb->fs);// Z
                        fgetc(sb->fs);// :
                        fscanf(sb->fs, "%s", rline->edits);
                    }
                }

            }

        }
        else
            break;
    */

    }

    // Update the block length
    sb->block_length = i;
    sb->reads->block_length = i;
    sb->QVs->block_length = i;

    return i;
}

uint32_t load_sam_line(sam_block sb){

    uint32_t j = 0;
    read_line rline = sb->reads->lines;
    qv_line qvline = sb->QVs->qv_lines;


    char buffer[1024];
    char *ptr;
    char *ID_line = *sb->IDs->IDs;
    char *rname_line = *sb->rnames->rnames;
    char *rnext = *sb->rnext->rnext;

    char **aux_fields = sb->aux->aux_str;

    rline->read_length = sb->read_length;
    qvline->columns = sb->read_length;
    // Read compulsory fields
    if (fgets(buffer, 1024, sb->fs)) {
        // ID
        ptr = strtok(buffer, "\t");
        strcpy(ID_line, ptr);
        // FLAG
        ptr = strtok(NULL, "\t");
        rline->invFlag = atoi(ptr);
        // RNAME
        ptr = strtok(NULL, "\t");
        strcpy(rname_line, ptr);
        // POS
        ptr = strtok(NULL, "\t");
        rline->pos = atoi(ptr);
        // MAPQ
        ptr = strtok(NULL, "\t");
        *sb->mapq->mapq = atoi(ptr);
        // CIGAR
        ptr = strtok(NULL, "\t");
        strcpy(rline->cigar, ptr);
        // RNEXT
        ptr = strtok(NULL, "\t");
        strcpy(rnext, ptr);
        // PNEXT
        ptr = strtok(NULL, "\t");
        *sb->pnext->pnext = atoi(ptr);
        // TLEN
        ptr = strtok(NULL, "\t");
        *sb->tlen->tlen = atoi(ptr);
        // SEQ
        ptr = strtok(NULL, "\t");
        strcpy(rline->read, ptr);
        sb->reads->models->read_length = (uint32_t)strlen(ptr);
        // QUAL
        sb->QVs->qv_lines->columns = (uint32_t)strlen(ptr);
        qvline->columns = (uint32_t)strlen(ptr);
        ptr = strtok(NULL, "\t");
        //printf("quality score read length sam file: %d\n", strlen(ptr));
        // Read the QVs and translate them to a 0-based scale
        // Check if the read is inversed
        if (rline->invFlag & 16) { // The read is inverted
            for (j = qvline->columns; j >= 1; j--) {
                qvline->data[j-1] = (*ptr) - 33, ptr++;
            }
        }
        else{ // The read is not inversed
            for (j = 0; j < qvline->columns; j++) {
                qvline->data[j] = (*ptr) - 33, ptr++;
            }
        }

        // Read the AUX fields until end of line, and store the MD field
        int auxCnt = 0;
        while( NULL != (ptr = strtok(NULL, "\t")) ){
            //MD:_:_ is a special case
            if (*ptr == 'M' && *(ptr+1) == 'D'){
                // skip MD:Z:
                //ptr += 5;
                strcpy(rline->edits, ptr);
                //break;
            } //else {
            strcpy(aux_fields[auxCnt], ptr);
            auxCnt++;
            //if we have reached the max. allowed aux fields, break.
            if(auxCnt==MAX_AUX_FIELDS) break;
            //}
        }

        //awful hack to check if the last field has a \n.
        if(auxCnt!=0) if(aux_fields[auxCnt-1][strlen(aux_fields[auxCnt-1])-1]=='\n') aux_fields[auxCnt-1][strlen(aux_fields[auxCnt-1])-1]=0;

        sb->aux->aux_cnt = auxCnt;
        return 0;
    }
    else {
        return 1;
    }
}

/**
 *
 *
 */
uint32_t load_qv_training_block(qv_block QV){

    qv_line qvline = QV->qv_lines;

    uint32_t j = 0, i = 0;

    long int oset = ftell(QV->fs);


    char buffer[1024];
    char *ptr;

    uint32_t invFlag;

    for (i = 0; i < QV->block_length; i++) {

        // Read compulsory fields
        if (fgets(buffer, 1024, QV->fs)) {
            // ID
            ptr = strtok(buffer, "\t");
            // FLAG
            ptr = strtok(NULL, "\t");
            invFlag = atoi(ptr);
            // RNAME
            ptr = strtok(NULL, "\t");
            // POS
            ptr = strtok(NULL, "\t");
            // MAPQ
            ptr = strtok(NULL, "\t");
            // CIGAR
            ptr = strtok(NULL, "\t");
            // RNEXT
            ptr = strtok(NULL, "\t");
            // PNEXT
            ptr = strtok(NULL, "\t");
            // TLEN
            ptr = strtok(NULL, "\t");
            // SEQ
            ptr = strtok(NULL, "\t");
            // QUAL
            ptr = strtok(NULL, "\t");
            // Read the QVs and translate them to a 0-based scale
            // Check if the read is inversed
            if (invFlag & 16) { // The read is inverted
                for (j = QV->columns; j >= 1; j--) {
                    qvline[i].data[j-1] = (*ptr) - 33, ptr++;
                }
            }
            else{ // The read is not inversed
                for (j = 1; j < QV->columns + 1; j++) {
                    qvline[i].data[j-1] = (*ptr) - 33, ptr++;
                }
            }
        }
        else {
            fseek(QV->fs, oset, SEEK_SET); //charlie
            return 1;
        }
    }

    fseek(QV->fs, oset, SEEK_SET);
    return 0;
}
