#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <getopt.h>

unsigned int modexp(unsigned int base, int exp, unsigned int mod)
{
  unsigned int result = 1;
  while(exp > 0)
    {
      if(exp & 1 == 1)
	result = (result * base) % mod;
      exp = exp >> 1;
      base = (base * base) % mod;
    }
  return result;
}

unsigned int modinv(unsigned int u, unsigned int v)
{
    unsigned int inv, u1, u3, v1, v3, t1, t3, q;
    int iter;
    /* Step X1. Initialise */
    u1 = 1;
    u3 = u;
    v1 = 0;
    v3 = v;
    /* Remember odd/even iterations */
    iter = 1;
    /* Step X2. Loop while v3 != 0 */
    while (v3 != 0)
    {
        /* Step X3. Divide and "Subtract" */
        q = u3 / v3;
        t3 = u3 % v3;
        t1 = u1 + q * v1;
        /* Swap */
        u1 = v1; v1 = t1; u3 = v3; v3 = t3;
        iter = -iter;
    }
    /* Make sure u3 = gcd(u,v) == 1 */
    if (u3 != 1)
        return 0;   /* Error: No inverse exists */
    /* Ensure a positive result */
    if (iter < 0)
        inv = v - u1;
    else
        inv = u1;
    return inv;
}


unsigned int rsa(unsigned int val, unsigned int *crt)
{
  unsigned int retval;
  unsigned int m1, m2, h;

  m1 = modexp(val,crt[2],crt[0]);
  m2 = modexp(val,crt[3],crt[1]);
  while(m1 < m2)
    m1 += crt[0];
  h = (crt[4]*(m1-m2))%crt[0];
  return m2 + h*crt[1];
  
}

void crtme(unsigned int *crt,unsigned int p, unsigned int q, unsigned int exp)
{
  crt[0] = p;
  crt[1] = q;
  crt[2] = exp%(crt[0] - 1);
  crt[3] = exp%(crt[1] - 1);
  crt[4] = modinv(crt[1],crt[0]);
}

void print_crt(unsigned int *crt)
{
  printf("p = %u, q = %u, dp = %u, dq = %u, q_inv = %u\n",crt[0], crt[1], crt[2], crt[3], crt[4]);
  printf("crt[] = {%u,%u,%u,%u,%u};\n",crt[0], crt[1], crt[2], crt[3], crt[4]);
}


struct option long_opts[] =
  {
    {"encrypt-key",1,NULL,'e'},
    {"decrypt-key",1,NULL,'d'},
    {"p",1,NULL,'p'},
    {"q",1,NULL,'q'},
    {NULL,0,NULL,0}
  };
const char short_opts[] = "d:e:p:q:";

int main(int argc, char **argv)
{
  
  unsigned int p = 137, q = 131,d = 11787,e = 3,crt[5],crte[5];
  
  char optval;

  while((optval = getopt_long(argc,argv,short_opts,long_opts,NULL)) > 0)
    {
      switch(optval)
	{
	case 'd':
	  sscanf(optarg,"%u",&d);
	  break;
	case 'e':
	  sscanf(optarg,"%u",&e);
	  break;
	case 'p':
	  sscanf(optarg,"%u",&p);
	  break;
	case 'q':
	  sscanf(optarg,"%u",&q);
	  break;
	default:
	  printf("%c -- Unknown flag.\n",optval);
	  exit(-1);
	}
    }
  

  crtme(crt,p,q,d);
  crtme(crte,p,q,e);
  
  printf("Chinese Remainder values for %u:\n",e);
  print_crt(crte);

  printf("Chinese Remainder values for %u:\n",d);
  print_crt(crt);

  printf("Example:\nPlain text = 8363\n");
  printf("Cipher text = %u\n",rsa(8363,crt));
  printf("Deciphered text = %u\n",rsa(rsa(8363,crt),crte));
  return 0;
  
}
