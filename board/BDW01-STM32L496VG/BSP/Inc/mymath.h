#ifndef __MYMATH_H
#define __MYMATH_H


void Ascii_to_hex_char(char *dstbuf,char*rstbuf);
void HexStrToByte(const char* source, unsigned char* dest, int sourceLen);  
void ByteToHexStr(const unsigned char* source, char* dest, int sourceLen);
void Hex2Str( const char *sSrc,  char *sDest, int nSrcLen );
int Myatoi(char *buf);
char * Myitoa(int val);
int str_to_hex(char *string, unsigned char *cbuf, int len);
void hex_to_str(char *ptr,unsigned char *buf,int len);


#endif



