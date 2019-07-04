# Makefile for building C programs

OBJ=main.o sam_models.o sam_file_allocation.o qv_codebook.o qv_compression.o read_compression.o read_decompression.o util.o well.o Arithmetic_stream.o stream_model.o pmf.o distortion.o quantizer.o sfio_ssh_session.o compression.o io_functions.o id_compression.o aux_compression.o edit.o

CC=g++
RM=rm -f

CXXFLAGS=-O3 -ffast-math -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -Wall -I../include -I/usr/local/include -I../libssh/build/src  -D__APPLE__ -std=c++11
LDFLAGS=-L/usr/local/lib -lc -lm -lpthread -L../libssh/build/src -lssh

%.o : %.c
	$(CC) -gdwarf-2 $(CXXFLAGS) -c $< 
%.o : %.cc
	$(CC) -gdwarf-2 $(CXXFLAGS) -c $< 


all : program

program : $(OBJ)
	$(CC) $(OBJ) -o program $(LDFLAGS)

debug : CXXFLAGS += -DDEBUG -ggdb
debug : program

clean :
	$(RM) *.o program
