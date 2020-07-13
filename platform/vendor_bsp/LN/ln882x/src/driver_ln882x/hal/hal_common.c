#include "utils/debug/log.h"
#include "hal/hal_common.h"

void assert_failed(uint8_t* file, uint32_t line)
{
    uint8_t *fileName = file;
    uint32_t lineNumber = line;
//    log_printf("ASSERT Failed-> file: %s @ line %d\r\n", fileName, lineNumber);
    while(1){
        fileName = fileName;
        lineNumber = lineNumber;
    };
}

