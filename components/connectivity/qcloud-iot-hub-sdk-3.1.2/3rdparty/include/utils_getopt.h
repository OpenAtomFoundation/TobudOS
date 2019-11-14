#ifndef __UTILS_GETOPT_H__
#define __UTILS_GETOPT_H__

extern char* utils_optarg;	/* pointer to argument of current option  */
int utils_getopt(int nargc, char* const* nargv, const char* options);

#endif 
