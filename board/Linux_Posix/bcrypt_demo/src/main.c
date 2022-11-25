#include "cmsis_os.h"
#include "bcrypt.h"

#define TASK1_STK_SIZE  512
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);

int Bcrypt(const char *Src, char *Dst)
{
    int Ret = -1;
    char Salt[BCRYPT_HASHSIZE] = {0};
    char Hash[BCRYPT_HASHSIZE] = {0};

    if(NULL == Src)
    {
        printf("Src is NULL!\n");
        goto ErrorHandler;
    }

    if(NULL == Dst)
    {
        printf("Dst is NULL!\n");
        goto ErrorHandler;
    }

    Ret = bcrypt_gensalt(12, Salt);
    if(Ret < 0)
    {
        printf("bcrypt_gensalt failed!\n");
        goto ErrorHandler;
    }

    Ret = bcrypt_hashpw(Src, Salt, Hash);
    if(Ret < 0)
    {
        printf("bcrypt_hashpw failed!\n");
        goto ErrorHandler;
    }

    memcpy(Dst, Hash, sizeof(Hash));

    return 0;

ErrorHandler:
    return -1;
}

void task1(void *arg)
{
    int Ret = -1;
    char Hash[BCRYPT_HASHSIZE] = {0};
    const char *DeviceKey = "8HLWBzFvOoZDo9wHZnCAKaAZeb8";

    Ret = Bcrypt(DeviceKey, Hash);
    if(Ret < 0)
    {
        printf("Bcrypt failed!\n");
        return ;
    }

    printf("Bcrypt DeviceKey = %s\n", Hash);
    
    while(1)
    {
        printf("BcryptTest success!\n");
        osDelay(1000);
    }
}

void application_entry(void *arg)
{
    osThreadCreate(osThread(task1), NULL); // Create task1
}

int main(void)
{
   osKernelInitialize(); //TOS Tiny kernel initialize
   application_entry(NULL);
   osKernelStart(); //Start TOS Tiny
   
   while (1)
   {
   }
}
