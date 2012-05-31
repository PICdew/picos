
typedef long crc24;

struct base64_stream
{
    crc24 crc;// CRC24 of the stream

    FILE *output;// output off buffer

    char buffer[3];
    char outblock[4];
    size_t bytes_in_buffer; 

};


