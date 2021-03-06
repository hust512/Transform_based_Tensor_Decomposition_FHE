/* Copyright (C) 2012-2017 IBM Corp.
 * This program is Licensed under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. See accompanying LICENSE file.
 */

/* Test_General.cpp - A general test program that uses a mix of operations over four ciphertexts.
 */
#include <NTL/ZZ.h>
#include <NTL/BasicThreadPool.h>
#include "FHE.h"
#include "timing.h"
#include "EncryptedArray.h"
#include <NTL/lzz_pXFactoring.h>
#include "Ctxt.h"
#include <stdio.h>
#include <cassert>
#include <cstdio>

//#ifdef DEBUG_PRINTOUT
//#define debugCompare(ea,sk,p,c) {\
//  NewPlaintextArray pp(ea);\
  ea.decrypt(c, sk, pp);\
  if (!equals(ea, pp, p)) { \
    std::cout << "oops:\n"; std::cout << p << "\n"; \
    std::cout << pp << "\n"; \
    exit(0); \
  }}
//#else
//#define debugCompare(ea,sk,p,c)
//#endif

/**************

1. c1.multiplyBy(c0)
2. c0 += random constant
3. c2 *= random constant
4. tmp = c1
5. ea.shift(tmp, random amount in [-nSlots/2, nSlots/2])
6. c2 += tmp
7. ea.rotate(c2, random amount in [1-nSlots, nSlots-1])
8. c1.negate()
9. c3.multiplyBy(c2)
10. c0 -= c3

**************/
int main(int argc, char **argv)
{
    //long pp=101;//plaintext base
    //long rr=1;//R-cipher times
    //long LL=4;//levels in the modulus chain
    //long cc=2;//number of columns in the key-switching matrices
    //long kk=80;//security param
    //long ss=0;//minimum number of slots  [ default=0 ]
    //long dd=0;// degree of the field extension
    //long ww=64;//
    //long mm = FindM(kk, LL, cc, pp, dd, ss, 0);//

  long mm,pp,rr,LL,cc,ww,dd,kk,ss;
   ww=64;
   rr=2;
   pp=3;
   cc=2;
   mm=0;
   kk=80;
   ss=0;
   dd=1;
   LL=1+NextPowerOfTwo(dd);
   //if (mm<2)
   //mm = FindM(/*secprm=*/80, LL, /*c=*/3, pp, 1, 0, mm, true);
  mm = FindM(kk, LL, cc, pp, dd, ss, 0);

  FHEcontext context(mm,pp,rr);//param setting
  buildModChain(context,pp,rr); //"new adding"
  cout<<context<<endl;
  //test receive
  fstream keyFile("/home/lowell/test.txt",fstream::out|fstream::trunc); //add test.txt fstream::out|fstream::trunc
  assert(keyFile.is_open());
  fstream keyFile1("/home/lowell/test1.txt", fstream::out|fstream::trunc);
  assert(keyFile1.is_open()); //save ciphertext
  //
  //buildModChain(context, LL, cc);
  ZZX G = context.alMod.getFactorsOverZZ()[0]; // a value G which is the monic polynomial used in constructing the ring BGV works

  //Key Genneration
  FHESecKey secretKey(context);
  const FHEPubKey& publicKey = secretKey;
  secretKey.GenSecKey(ww); // A Hamming-weight-w secret key
  addSome1DMatrices(secretKey);

  //Encryption
  EncryptedArray ea(context, G);

  //long nslots=ea.size();
  //Use a PlaintextArray to represent a plaintext

  NewPlaintextArray p1(ea);//++
  random(ea,p1); //random param
  p1.print(keyFile);

  //cout<<p0<<endl;
  Ctxt c(publicKey);//++
  //ZZX ploy=to_ZZX(6);
  //ea.encode(ploy,p1);//++
  ea.encrypt(c, publicKey, p1);//++//plaintxt values p1, ciphertxt values c
  //c.print(keyFile);
  keyFile1<<c;
  cout<<c[0];
  //cout<<c. getContext();//FHE params

  //Decryption
  NewPlaintextArray p_decrypted(ea);//++
  ea.decrypt(c, secretKey, p_decrypted);//++decrypted ciphertxt c ,nonally p_decrypted == p1
  //cout<<c;
  //cout<<c.getPtxtSpace();c is ciphertext!
  //cout<<c.getContext();
 if (equals(ea,p1,p_decrypted))
      cout<<"equal!"<<endl;
 else
     cerr<<"oops 0"<<endl;
}
