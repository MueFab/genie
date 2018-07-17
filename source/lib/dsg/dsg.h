void create_access_unit()
{
    


void encode_descriptor_streams(
    const struct DescriptorStreams& descriptorStreams)
{
    encode_pos_stream(descriptorStreams->pos, encoded_pos_sequences);

    write_pos_block(encoded_pos_sequences);
}


void write_block()
{
    write_block_header();
    write_block_payload();
}



void write_block_header(
    descriptor_ID,
    block_size);

void write_pos_block_header()
{
    wrtie_block_header(0, *encoded_pos_sequence_size);
}

void write_block_header(
    u8 descriptor_ID,
    u32 block_size)
{
    // Write descriptor_ID = 0 as u(7)

    // Write reserved == 0 as u(1)

    // Write block_size == *encoded_pos_sequence_size
}

void encode_pos_stream(
    const std::vector<pos_type>& pos_stream
    byte_type * const encoded_pos_sequence,
    size_t * const encoded_pos_sequence_size);



struct descriptorStreams {
    std::vector<pos_type>& pos_stream;
}

void create_descriptor_streams_from_fastq(
    const std::vector<FastqRecords>& fastqRecords,
    struct descriptorStreams * const descriptorStreams)
{
    create_pos_stream_from_fastq(fastqRecords, &(descriptorStreams->pos_stream));
}







void createDescriptorStreamSetFromUnalignedData()

{


void createBlock(
    descriptor)
{
    block_header.descriptor_ID =
    block_header
    
}



FromFastaRecordBlock(
    const std::vector<FastaRecord> fastaRecordBlock,
    DescriptorStreamSet * const descriptorStreamSet);

void create

struct DescriptorStreamSetClassU {
    pos;
    rcomp;
    flags;
    mmpos;
    mmtype;
    clips;
    ureads;
    rlen;
    rtype;
}
