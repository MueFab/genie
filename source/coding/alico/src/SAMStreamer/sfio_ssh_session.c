//
//  sfio_ssh_session.c
//  XC_s2fastqIO
//
//  Created by Mikel Hernaez on 12/1/14.
//  Copyright (c) 2014 Mikel Hernaez. All rights reserved.
//
#include <assert.h>

#include <stdio.h>

#include <errno.h>
#include <libssh/sftp.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "sam_block.h"

int verify_knownhost(ssh_session session)
{
    int state, rc;
    unsigned char *hash = NULL;
    char *hexa;
    char buf[10];
    size_t hlen;
    ssh_key srv_pubkey;
    
    state = ssh_is_server_known(session);
    if (ssh_get_publickey(session, &srv_pubkey) < 0) {
        return -1;
    }
    rc = ssh_get_publickey_hash(srv_pubkey, SSH_PUBLICKEY_HASH_SHA1, &hash, &hlen);
    ssh_key_free(srv_pubkey);
    if (rc < 0) {
        return -1;
    }
    switch (state)
    {
        case SSH_SERVER_KNOWN_OK:
            break; /* ok */
        case SSH_SERVER_KNOWN_CHANGED:
            fprintf(stderr, "Host key for server changed: it is now:\n");
            ssh_print_hexa("Public key hash", hash, hlen);
            fprintf(stderr, "For security reasons, connection will be stopped\n");
            free(hash);
            return -1;
        case SSH_SERVER_FOUND_OTHER:
            fprintf(stderr, "The host key for this server was not found but an other"
                    "type of key exists.\n");
            fprintf(stderr, "An attacker might change the default server key to"
                    "confuse your client into thinking the key does not exist\n");
            free(hash);
            return -1;
        case SSH_SERVER_FILE_NOT_FOUND:
            fprintf(stderr, "Could not find known host file.\n");
            fprintf(stderr, "If you accept the host key here, the file will be"
                    "automatically created.\n");
            /* fallback to SSH_SERVER_NOT_KNOWN behavior */
        case SSH_SERVER_NOT_KNOWN:
            hexa = ssh_get_hexa(hash, hlen);
            fprintf(stderr,"The server is unknown. Do you trust the host key?\n");
            fprintf(stderr, "Public key hash: %s\n", hexa);
            free(hexa);
            if (fgets(buf, sizeof(buf), stdin) == NULL)
            {
                free(hash);
                return -1;
            }
            if (strncmp(buf, "yes", 3) != 0)
            {
                free(hash);
                return -1;
            }
            if (ssh_write_knownhost(session) < 0)
            {
                fprintf(stderr, "Error %s\n", strerror(errno));
                free(hash);
                return -1;
            }
            break;
        case SSH_SERVER_ERROR:
            fprintf(stderr, "Error %s", ssh_get_error(session));
            free(hash);
            return -1;
    }
    free(hash);
    return 0;
}



int scp_read(ssh_session session, char* directory)
{
    ssh_scp scp;
    int rc;
    scp = ssh_scp_new
    (session, SSH_SCP_READ | SSH_SCP_RECURSIVE, directory);
    if (scp == NULL)
    {
        fprintf(stderr, "Error allocating scp session: %s\n",
                ssh_get_error(session));
        return SSH_ERROR;
    }
    rc = ssh_scp_init(scp);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error initializing scp session: %s\n",
                ssh_get_error(session));
        ssh_scp_free(scp);
        return rc;
    }
    
    uint64_t length = 0;
    char filepath[1024] = "/tmp/idoFiles/";
    char *pathEnd, *filename, *buffer;
    FILE *fp;
    uint32_t bufCtr = 0;
    pathEnd = filepath + strlen(filepath);
    
    rc = ssh_scp_pull_request(scp);
    if (rc != SSH_SCP_REQUEST_NEWDIR)
    {
        fprintf(stderr, "Error receiving information about Directory: %s\n",
                ssh_get_error(session));
        return SSH_ERROR;
    }
    
    rc = ssh_scp_accept_request(scp);
    
    do{
        
        rc = ssh_scp_pull_request(scp);
        if (rc != SSH_SCP_REQUEST_NEWFILE)
        {
            fprintf(stderr, "Error receiving information about file: %s\n",
                    ssh_get_error(session));
            continue;
        }
        length = ssh_scp_request_get_size(scp);
        filename = strdup(ssh_scp_request_get_filename(scp));
        int mode = ssh_scp_request_get_permissions(scp);
        printf("Receiving file %s, size %lu, permisssions 0%o\n",
               filename, length, mode);
        
        buffer = (char *) malloc(length);
        if (buffer == NULL)
        {
            fprintf(stderr, "Memory allocation error\n");
            return SSH_ERROR;
        }
        
        // Accept the scp request
        rc = ssh_scp_accept_request(scp);
        
        // Read the file in chuncks
        do {
            rc = ssh_scp_read(scp, buffer+bufCtr, length);
            bufCtr += rc;
            if (rc == SSH_ERROR)
            {
                fprintf(stderr, "Error receiving file data: %s\n", ssh_get_error(session));
                free(buffer);
                free(filename);
                return rc;
            }
            
        }while (bufCtr < length);
        
        printf("Done: Num of Bytes read %d, file length %lu\n", bufCtr, length);
        
        strcat(filepath, filename);
        if ((fp = fopen(filepath, "w")) == NULL) {
            return SSH_ERROR;
        }
        
        fwrite(buffer, sizeof(char), bufCtr, fp);
        
        free(buffer);
        free(filename);
        fclose(fp);
        bufCtr = 0;
        *pathEnd = 0;
        
        file_available++;
        
    }while(length != 0);
    
    // Verify that there are no more files to read
    /*rc = ssh_scp_pull_request(scp);
    if (rc != SSH_SCP_REQUEST_EOF)
    {
        fprintf(stderr, "Unexpected request: %s\n",
                ssh_get_error(session));
        return SSH_ERROR;
    }
    */
    // No more files to read
    rc = ssh_scp_close(scp);
    ssh_scp_free(scp);
    return SSH_OK;
}



int scp_write(ssh_session session, char* directory)
{
    ssh_scp scp;
    int rc;
    scp = ssh_scp_new
    (session, SSH_SCP_WRITE | SSH_SCP_RECURSIVE, directory);
    if (scp == NULL)
    {
        fprintf(stderr, "Error allocating scp session: %s\n",
                ssh_get_error(session));
        return SSH_ERROR;
    }
    rc = ssh_scp_init(scp);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error initializing scp session: %s\n",
                ssh_get_error(session));
        ssh_scp_free(scp);
        return rc;
    }
    
    uint64_t length = 0;
    char filename[1024];
    char filepath[1024];
    char readyname[256];
    char *pathEnd, *nameEnd;
    char buffer[IO_STREAM_BUF_LEN];
    FILE *fp;
    
    uint32_t ctr = 0;
    
    strcpy(filepath, "/tmp/idoFiles/idoFile.");
    pathEnd = filepath + strlen("/tmp/idoFiles/idoFile.");
    
    strcpy(filename, "idoFile.");
    nameEnd = filename + strlen("idoFile.");
    
    printf("Uploading files to %s\n", directory);
    do{
        while (file_available == 0);
        //printf("%d\n", file_available);
        sprintf(pathEnd, "%010d", ctr);
        sprintf(nameEnd, "%010d", ctr++);
        
        if ((fp = fopen(filepath, "r")) == NULL) {
            length = 1;
            printf(" ERROR: File not available");
        }
        
        fseek(fp, 0L, SEEK_END);
        length = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        
        fread(buffer, sizeof(char), length, fp);
        
        rc = ssh_scp_push_file(scp, filename, length, S_IRUSR |  S_IWUSR);
        if (rc != SSH_OK)
        {
            fprintf(stderr, "Can't open remote file: %s\n",
                    ssh_get_error(session));
            return rc;
        }
        rc = ssh_scp_write(scp, buffer, length);
        
        if (rc != SSH_OK)
        {
            fprintf(stderr, "Can't write to remote file: %s\n",
                    ssh_get_error(session));
            return rc;
        }
        printf("File %s uploaded\n", filename);
        
        // Upload the "ready" file
        strcpy(readyname, filename);
        strcat(readyname, "_ready");
        rc = ssh_scp_push_file(scp, readyname, 1, S_IRUSR |  S_IWUSR);
        if (rc != SSH_OK)
        {
            fprintf(stderr, "Can't open remote file: %s\n",
                    ssh_get_error(session));
            return rc;
        }
        rc = ssh_scp_write(scp, buffer, 1);
        
        if (rc != SSH_OK)
        {
            fprintf(stderr, "Can't write to remote file: %s\n",
                    ssh_get_error(session));
            return rc;
        }
        printf("File %s uploaded\n", readyname);
        
        
        file_available--;
        
    }while(length != 0);
    
    rc = ssh_scp_close(scp);
    ssh_scp_free(scp);
    return SSH_OK;
}


ssh_session open_ssh_session(char* host_name, char *username){
    
    ssh_session my_ssh_session;
    int rc;
    char *password;
    int verbosity = SSH_LOG_NOLOG;
    //int verbosity = SSH_LOG_PROTOCOL;

    
    // Open session and set options
    my_ssh_session = ssh_new();
    if (my_ssh_session == NULL)
        exit(-1);
    
    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, host_name);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, username);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    
    // Connect to server
    rc = ssh_connect(my_ssh_session);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error connecting to server: %s\n",
                ssh_get_error(my_ssh_session));
        ssh_free(my_ssh_session);
        exit(-1);
    }
    
    // Verify the server's identity
    // For the source code of verify_knowhost(), check above
    if (verify_knownhost(my_ssh_session) < 0)
    {
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        exit(-1);
    }
    
    // Authenticate ourselves
    password = getpass("Password: ");
    rc = ssh_userauth_password(my_ssh_session, NULL, password);
    if (rc != SSH_AUTH_SUCCESS)
    {
        fprintf(stderr, "Error authenticating with password: %s\n",
                ssh_get_error(my_ssh_session));
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        exit(-1);
    }

    return my_ssh_session;
    
}

void* upload(void* remote_info){
    
    struct remote_file_info *info = (struct remote_file_info *) remote_info;
    ssh_session my_ssh_session;
    
    char hostname[1024];
    char username[1024];
    char directory[1024];
    
    strcpy(hostname, info->host_name);
    strcpy(username, info->username);
    strcpy(directory, info->filename);
    
    my_ssh_session = open_ssh_session(hostname, username);
    
    printf("Uploading file to %s@%s:%s/...\n", username, hostname, directory);
    scp_write(my_ssh_session, directory);
    
    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);
    
    return NULL;
}

void* download(void* remote_info){
    
    struct remote_file_info *info = (struct remote_file_info *) remote_info;
    ssh_session my_ssh_session;
    time_t begin;
    time_t end;
    
    time(&begin);
    my_ssh_session = open_ssh_session(info->host_name, info->username);
    
    printf("Downloading file from %s@%s...\n", info->host_name, info->username);
    scp_read(my_ssh_session, info->filename);
    
    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);
    
    time(&end);
    
    printf("Downloading time elapsed: %ld seconds\n", end-begin);
    
    return NULL;
}

void* remote_decompression(void* remote_info){
    
    // TODO!!!
    
    struct remote_file_info *info = (struct remote_file_info *) remote_info;
    ssh_session my_ssh_session;
    
    char hostname[1024];
    char username[1024];
    char directory[1024];
    
    strcpy(hostname, info->host_name);
    strcpy(username, info->username);
    strcpy(directory, info->filename);
    
    ssh_channel channel;
    int rc;
    char buffer[256];
    
    my_ssh_session = open_ssh_session(hostname, username);
    
    channel = ssh_channel_new(my_ssh_session);
    if (channel == NULL)
        return NULL;
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return NULL;
    }
    
    strcat(buffer, "sfio -r foo.foo ");
    strcat(buffer, directory);
    strcat(buffer, "~/samFastqIO/ref/chr20");
    
    rc = ssh_channel_request_exec(channel, buffer);

    
    printf("Uploading file to %s@%s:%s/...\n", username, hostname, directory);
    
   
    //rc = ssh_channel_request_exec(channel, "touch writetest.txt");
    
    //nwritten = ssh_channel_write(channel, "ls\n", 256);
    //rc = ssh_channel_request_exec(channel, "cat >> writetest.txt");
    //nbytes = read(0, buffer, sizeof(buffer));
    
    
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return NULL;
    }

    
    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);
    
    return NULL;
}
