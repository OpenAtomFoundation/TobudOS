/**************************************************************************
Copyright (C) 2009 Lander Casado, Philippas Tsigas

All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files 
(the "Software"), to deal with the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish, 
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions: 

Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimers. Redistributions in
binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimers in the documentation and/or 
other materials provided with the distribution.

In no event shall the authors or copyright holders be liable for any special,
incidental, indirect or consequential damages of any kind, or any damages 
whatsoever resulting from loss of use, data or profits, whether or not 
advised of the possibility of damage, and on any theory of liability, 
arising out of or in connection with the use or performance of this software.
 
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
DEALINGS WITH THE SOFTWARE

*****************************************************************************/

#ifndef _CMAC_H_
#define _CMAC_H_

#ifdef __cplusplus
extern "C"
#endif

#include "aes.h" 
  
#define AES_CMAC_KEY_LENGTH     16
#define AES_CMAC_DIGEST_LENGTH  16
 
typedef struct _AES_CMAC_CTX {
            aes_context    rijndael;
            uint8_t        X[16];
            uint8_t        M_last[16];
            uint32_t       M_n;
    } AES_CMAC_CTX;
   
//#include <sys/cdefs.h>
    
//__BEGIN_DECLS
void     AES_CMAC_Init(AES_CMAC_CTX * ctx);
void     AES_CMAC_SetKey(AES_CMAC_CTX * ctx, const uint8_t key[AES_CMAC_KEY_LENGTH]);
void     AES_CMAC_Update(AES_CMAC_CTX * ctx, const uint8_t * data, uint32_t len);
          //          __attribute__((__bounded__(__string__,2,3)));
void     AES_CMAC_Final(uint8_t digest[AES_CMAC_DIGEST_LENGTH], AES_CMAC_CTX  * ctx);
            //     __attribute__((__bounded__(__minbytes__,1,AES_CMAC_DIGEST_LENGTH)));
//__END_DECLS

#ifdef __cplusplus
}
#endif

#endif /* _CMAC_H_ */

