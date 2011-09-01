/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * This file performs rsa encryption using integer size bit precision.
 */
#include <stdio.h>
#include <math.h>


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
