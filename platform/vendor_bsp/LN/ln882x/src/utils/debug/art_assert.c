#include "ln88xx.h"
#include "utils/debug/art_assert.h"
#include "utils/debug/log.h"

void __aeabi_assert(const char *expr,const char *file,int line)
{
  __disable_irq();
  LOG(LOG_LVL_ERROR,"Assertion Failed: file %s, line %d, %s\n",file,line,expr);
  __BKPT(0);
  while(1);
}

