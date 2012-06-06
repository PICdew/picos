#include "base64.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>

#define CRC24_INIT 0xB704CEL
#define CRC24_POLY 0x1864CFBL

typedef long crc24;
crc24 base64_crc_octets(struct base64_stream *encoder,unsigned char *octets, size_t len)
{
    int i;

   if(encoder == NULL)
      reason_exit("base64_crc_octets: NULL pointer for encoding stream object\n"); 

    while (len--) {
         encoder->crc ^= (*octets++) << 16;
         for (i = 0; i < 8; i++) {
            encoder->crc <<= 1;
            if (encoder->crc & 0x1000000)
                 encoder->crc ^= CRC24_POLY;
         }
    }
   return encoder->crc & 0xFFFFFFL;
}

static const char base64_values[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
int base64_encode_buffer(struct base64_stream *encoder)
{
    char *buffer, *outblock;
    if(encoder == NULL || encoder->outblock == NULL|| encoder->buffer == NULL)
        return -1;

    outblock = encoder->outblock;
    buffer = encoder->buffer;

   outblock[0] = (buffer[0] & 0xfc) >> 2;
   full_assert(outblock[0] < 64,"The buffer byte managed to exceed base64 size.\n");
   outblock[0] = base64_values[outblock[0]];
   outblock[1] = ((buffer[0] & 0x3) << 4) | ((buffer[1] & 0xf0) >> 4);
   full_assert(outblock[1] < 64,"The buffer byte managed to exceed base64 size.\n");
   outblock[1] = base64_values[outblock[1]];
   outblock[2] = ((buffer[1] & 0xf) << 2) | ((buffer[2] & 0xc0) >> 6);
   full_assert(outblock[2] < 64,"The buffer byte managed to exceed base64 size.\n");
   outblock[2] = base64_values[outblock[2]];
   outblock[3] = buffer[2] & 0x3f;
   full_assert(outblock[3] < 64,"The buffer byte managed to exceed base64 size.\n");
   outblock[3] = base64_values[outblock[3]];

   return 0;
}

char base64_decode_byte(char byte)
{
    if(byte >= 'A' && byte <= 'Z')
        return byte - 'A';
    if(byte >= 'a' && byte <= 'z')
        return (byte - 'a') + 26;
    if(byte >= '0' && byte <= '9')
        return (byte - '0') + 52;
    if(byte == '+')
        return 62;
    if(byte == '/')
        return 63;
    if(byte == '=')
        return 64;

    reason_exit("base64_decode_byte: Invalid base64 character '%c'\n",byte);
}

int base64_decode_buffer(struct base64_stream *encoder)
{
    char *buffer, *outblock, tmp;
    if(encoder == NULL || encoder->outblock == NULL|| encoder->buffer == NULL)
        return -1;

    outblock = encoder->outblock;
    buffer = encoder->buffer;

    buffer[0] = base64_decode_byte(outblock[0]) << 2;
    tmp = base64_decode_byte(outblock[1]);
    buffer[0] |= (tmp & 0x30) >> 4;
    buffer[1] = (tmp & 0x0f) << 4;
    tmp = base64_decode_byte(outblock[2]);
    if(tmp == 64)
    {
        buffer[1] = buffer[2] = 0;
        return 0;
    }
    buffer[1] |= (tmp & 0x3c) >> 2;
    buffer[2] = (tmp & 0x3) << 6;
    tmp = base64_decode_byte(outblock[3]);
    if(tmp == 64)
    {
        buffer[2] = 0;
        return 0;
    }
    buffer[2] |= tmp;

    return 0;
} 

void base64_decode(struct base64_stream *encoder, void *data, size_t size)
{
    FILE *output;
    char amount_to_buffer;
    if(encoder == NULL || data == NULL)
        return;

    if(encoder->output == NULL || size == 0)
        return;

    output = encoder->output;
    encoder->encode = false;
    for(;size > 0;size-=4)
    {
        full_assert(encoder->bytes_in_buffer < 4,"Internal Error: bytes_in_buffer should never exceed 4. It equals %d\n",encoder->bytes_in_buffer);
        amount_to_buffer = ((size > 4)? 4 : size) - encoder->bytes_in_buffer;
        memcpy(&encoder->outblock[encoder->bytes_in_buffer],data,amount_to_buffer);

        encoder->bytes_in_buffer = (size >= 4)? 0 : size;
        if(encoder->bytes_in_buffer == 0)
        {
            base64_decode_buffer(encoder);
            base64_crc_octets(encoder,encoder->buffer,3);
            fprintf(output,"%c%c%c",encoder->buffer[0],encoder->buffer[1],encoder->buffer[2]);
            fflush(output);
            data += amount_to_buffer;
            memset(encoder->buffer,0,3*sizeof(char));
            memset(encoder->outblock,0,4*sizeof(char));
        }
        else
            break;
    }
} 

void base64_encode(struct base64_stream *encoder, void *data, size_t size)
{
    FILE *output;
	char amount_to_buffer;

    if(encoder == NULL || data == NULL)
        return;

    if(encoder->output == NULL || size == 0)
        return;

    encoder->encode = true;

    output = encoder->output;

    for(;size > 0;size-=3)
    {
        full_assert(encoder->bytes_in_buffer < 3,"Internal Error: bytes_in_buffer should never exceed 3. It equals %d\n",encoder->bytes_in_buffer);
		amount_to_buffer = ((size > 3)? 3 : size) - encoder->bytes_in_buffer;
       memcpy(&encoder->buffer[encoder->bytes_in_buffer],data,amount_to_buffer);

       encoder->bytes_in_buffer = (size >= 3)? 0 : size;
       if(encoder->bytes_in_buffer == 0)
       {
          base64_encode_buffer(encoder);
          base64_crc_octets(encoder,encoder->buffer,3);
          fprintf(output,"%c%c%c%c",encoder->outblock[0],encoder->outblock[1],encoder->outblock[2],encoder->outblock[3]);
          fflush(output);
          data += amount_to_buffer;
           memset(encoder->buffer,0,3*sizeof(char));
           memset(encoder->outblock,0,4*sizeof(char));
       }
       else
           break;
    }

}
int base64_flush(struct base64_stream *encoder)
{
    if(encoder == NULL)
        return 0;

    if(encoder->bytes_in_buffer != 0)
    {
        if(encoder->encode)
        {
            if(encoder->bytes_in_buffer == 2)
            {
                encoder->buffer[2] = 0;
                base64_crc_octets(encoder,encoder->buffer,2);
            }
            if(encoder->bytes_in_buffer == 1)
            {
                encoder->buffer[2] = encoder->buffer[1] = 0;
                base64_crc_octets(encoder,encoder->buffer,1);
            }
            if(base64_encode_buffer(encoder) == 0 && encoder->output != NULL)// this checks whether outblock is null 
            {
                if(encoder->bytes_in_buffer == 2)
                    encoder->outblock[3] = '=';
                if(encoder->bytes_in_buffer == 1)
                    encoder->outblock[2] = encoder->outblock[3] = '=';
                fprintf(encoder->output,"%c%c%c%c",encoder->outblock[0],encoder->outblock[1],encoder->outblock[2],encoder->outblock[3]);         
            }

        }
    }

    if(encoder->output != NULL)
    {
        fprintf(encoder->output,"\n=%ld", encoder->crc);
        fflush(encoder->output);
    }

}

int base64_close(struct base64_stream *encoder)
{
    if(encoder == NULL)
        return 0;

    encoder->crc = 0xdead;
}

int base64_init(struct base64_stream *obj, FILE *output)
{
    if(obj == NULL)
        return -1;

    obj->crc = CRC24_INIT;
    obj->output = output;
    obj->bytes_in_buffer = 0;
    obj->encode = true;
    memset(obj->buffer,0,3*sizeof(char));
    memset(obj->outblock,0,4*sizeof(char));

    return 0;
}

#ifdef BASE64_MAIN
int main(int argc, char **argv)
{
    int counter = 1;
    struct base64_stream encoder;

    base64_init(&encoder,stdout);

    if(argc == 1)
        return 0;

    if(strcmp(argv[counter],"-e") == 0)
    {
        counter++;
    }
    else if(strcmp(argv[counter],"-d") == 0)
    {
        encoder.encode = false;
        counter++;
    }
   
    for(;counter < argc;counter++)
    { 
        if(encoder.encode)
            base64_encode(&encoder,argv[counter],strlen(argv[counter]));
        else
            base64_decode(&encoder,argv[counter],strlen(argv[counter]));
        fflush(stdout);
    }

    base64_flush(&encoder);
    base64_close(&encoder);

    return 0;    

}
#endif
