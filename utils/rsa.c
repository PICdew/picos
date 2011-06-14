#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <getopt.h>
#include "../rsa.h"

#define RSA_BLOCK_SIZE 8

void crtme(unsigned int *crt,unsigned int p, unsigned int q, unsigned int exp)
{
  if(crt == NULL)
    return;
  crt[0] = p;
  crt[1] = q;
  crt[2] = exp%(crt[0] - 1);
  crt[3] = exp%(crt[1] - 1);
  crt[4] = modinv(crt[1],crt[0]);
}

void rsa_message(unsigned int *cipher,const char *message,
		 unsigned int *keys, int should_encrypt)
{
  unsigned int curr;
  size_t pos = 0;
  if(cipher == NULL || keys == NULL || message == NULL)
    return;
  while(message != NULL && strlen(message) != 0)
    {
      curr = ((unsigned int) *message);
      if(should_encrypt)// salt
	  curr += pos;
      cipher[pos++] = rsa(curr,keys);
      if(should_encrypt == 0)// unsalted
	cipher[pos - 1] -= pos + 1;
      if(strlen(message) == 1)
	message = NULL;
      else
	message++;
    }
      
}

void rsa_stream(unsigned int *cipher,const char *message,
		 unsigned int *keys, int should_encrypt)
{
  unsigned int curr;
  size_t pos = 0;
  char block[RSA_BLOCK_SIZE];
  if(cipher == NULL || keys == NULL || message == NULL)
    return;
  while(message != NULL && strlen(message) != 0)
    {
      strncpy(block,message,RSA_BLOCK_SIZE);
      sscanf(block,"%8u",&curr);
      if(should_encrypt)
	curr += pos;
      cipher[pos++] = rsa(curr,keys);
      if(should_encrypt == 0)
	cipher[pos - 1] -= pos - 1;
      if(strlen(message) < RSA_BLOCK_SIZE )
	message = NULL;
      else
	message += RSA_BLOCK_SIZE;
    }
      
}


void print_crt(unsigned int *crt)
{
  printf("p = %u, q = %u, dp = %u, dq = %u, q_inv = %u\n",crt[0], crt[1], crt[2], crt[3], crt[4]);
  printf("crt[] = {%u,%u,%u,%u,%u};\n",crt[0], crt[1], crt[2], crt[3], crt[4]);
}

enum {RSA_ENCRYPT_KEY,RSA_DECRYPT_KEY};
struct option long_opts[] =
  {
    {"cipher-only",0,NULL,'c'},
    {"decrypt",0,NULL,'d'},
    {"encrypt",0,NULL,'e'},
    {"encrypt-key",1,RSA_ENCRYPT_KEY},
    {"decrypt-key",1,NULL,RSA_DECRYPT_KEY},
    {"help",0,NULL,'h'},
    {"message",1,NULL,'m'},
    {"p",1,NULL,'p'},
    {"q",1,NULL,'q'},
    {"stream-in",0,NULL,'s'},
    {"stream-out",0,NULL,'S'},
    {NULL,0,NULL,0}
  };
const char short_opts[] = "Scdehm:p:q:s:";

void print_help()
{
  struct option *opt = long_opts;
  printf("rsa -- RSA for pic\n");
  printf("Copyright 2011 David Coss, PhD\n");
  printf("-------------------------------\n");
  printf("RSA encryption for use with the pic microcontroler.\n");
  printf("\n");
  printf("Usage: ./rsa [options]\n");
  printf("\n");
  printf("Options:\n");
  
  while(opt != NULL && opt->name != NULL)
    {
      printf("--%s",opt->name);
      if(opt->val >= 'a' && opt->val <= 'z')
	printf(", -%c ",opt->val);
      else
	printf(" ");
      if(opt->has_arg)
	printf("<ARG>  ");
      else
	printf("       ");
      printf("\n\t");
      switch(opt->val)
	{
	case 'c':
	  printf("Output only the encrypted/decrypted result.");
	  break;
	case 'd':
	  printf("Decrypt input.");
	  break;
	case 'e':
	  printf("Encrypt input.");
	  break;
	case RSA_ENCRYPT_KEY:
	  printf("Specify the encryption key.");
	  break;
	case RSA_DECRYPT_KEY:
	  printf("Specify the decryption key.");
	  break;
	case 'h':
	  printf("Display this message.");
	  break;
	case 'm':
	  printf("Character string message to encrypt/decrypt.");
	  break;
	case 'p':
	  printf("Specify the larger prime factor of the modulus.");
	  break;
	case 'q':
	  printf("Specify the smaller prime factor of the modulus.");
	  break;
	case 's':
	  printf("Use an input stream rather than character string.");
	  break;
	case 'S':
	  printf("Display output as a stream rather than character string.");
	  break;
	default:
	  printf("??");
	  break;
	}
      printf("\n");
      opt++;
    }
}

int main(int argc, char **argv)
{
  
  unsigned int p = 137, q = 131,d = 11787,e = 3,crt[5],crte[5];
  unsigned int c;
  const char *message = strdup("Hello, World!");
  int should_encrypt = 1, use_stream = 0, output_stream = 0;
  int cipher_only = 0;
  
  char optval;
  
  while((optval = getopt_long(argc,argv,short_opts,long_opts,NULL)) > 0)
    {
      switch(optval)
	{
	case 'S':
	  output_stream = 1;
	  break;
	case 'c':
	  cipher_only = 1;
	  break;
	case 'd':
	  should_encrypt = 0;
	  break;
	case RSA_DECRYPT_KEY:
	  sscanf(optarg,"%u",&d);
	  break;
	case RSA_ENCRYPT_KEY:
	  sscanf(optarg,"%u",&e);
	  break;
	case 'h':
	  print_help();
	  exit(0);
	case 'e':
	  should_encrypt = 1;
	  break;
	case 'm':
	  message = optarg;
	  break;
	case 'p':
	  sscanf(optarg,"%u",&p);
	  break;
	case 'q':
	  sscanf(optarg,"%u",&q);
	  break;
	case 's':
	  message = optarg;
	  use_stream = 1;
	  break;
	default:
	  printf("%c -- Unknown flag.\n",optval);
	  exit(-1);
	}
    }
  
  crtme(crt,p,q,d);
  crtme(crte,p,q,e);

  if(!cipher_only)
    {
      unsigned int mval = (unsigned int)*message;
      c = rsa(mval,crte);
      printf("Chinese Remainder values for %u:\n",e);
      print_crt(crte);
      printf("Chinese Remainder values for %u:\n",d);
      print_crt(crt);
      printf("Example:\nPlain text = %u\n",mval);
      printf("Cipher text = %u\n",c);
      printf("Deciphered text = %u\n",rsa(c,crt));
    }
  else
    {
      unsigned int *keys;
      size_t num_blocks = (size_t)ceil(strlen(message)/2);
      unsigned int cipher[num_blocks];
      if(should_encrypt)
	keys = crte;
      else
	keys = crt;
    
      if(use_stream)
	rsa_stream(cipher,message,keys,should_encrypt);
      else
	rsa_message(cipher,message,keys,should_encrypt);
      if(output_stream)
	printf("%08u%08u%08u%08u",cipher[0],cipher[1],cipher[2],cipher[3]);
      else
	printf("%c%c%c%c",0xff&cipher[0],(char)0xff&cipher[1],(char)0xff&cipher[2],(char)0xff&cipher[3]);

      return c;
    }
  return 0;
}
