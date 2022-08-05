#include "cmsis_os.h"
#include "iniparser.h"
#define CONFIG_NAME "Config.ini"

struct DataInfo_t
{
    int InitData;
    int VolumeData;
    int LanguageVersion;
};

#define TASK1_STK_SIZE  512
void task1(void *arg);
osThreadDef(task1, osPriorityNormal, 1, TASK1_STK_SIZE);

void task1(void *arg)
{
    int Len = -1;
    int Ret = -1;
    char Buf[128];
    char *DataPtr = NULL;
    struct DataInfo_t Data;
    FILE *IniTest = NULL ;
    FILE *DefaultIni = NULL;
    dictionary  *ConfigIni = NULL;
    
    /*1. Create ini config file*/
    IniTest = fopen(CONFIG_NAME, "w");
    if(NULL == IniTest)
    {
        printf("IniTest is Null!\n");
        return ;
    }
    
    fprintf(IniTest,
        "[Setting]\n"
        "init_data=0;\n"
        "volume_data=1;\n"
        "language_version=1;\n"
    );
    
    Ret = fclose(IniTest);
    if(Ret != 0)
    {
        printf("close IniTest fail!\n");
        return ;
    }
    
    IniTest = NULL;
    
    IniTest = fopen(CONFIG_NAME, "r");
    if(NULL == IniTest)
    {
        printf("IniTest is Null!\n");
        return ;
    }
    
    memset(Buf, 0, sizeof(Buf));
    while(fgets(Buf, sizeof(Buf), IniTest))
    {
        printf("Buf: %s", Buf);
    }
    
    fclose(IniTest);
    IniTest = NULL;
    
    putchar('\n');
    
    /*2. Test read ini config file data*/
    ConfigIni = iniparser_load(CONFIG_NAME);
    if(NULL == ConfigIni)
    {
        printf("ConfigIni is NULL!\n");
        return ;
    }
    
    //iniparser_dump(ConfigIni, stderr);
    Data.InitData = iniparser_getint(ConfigIni,"Setting:init_data",-1);
    if(-1 == Data.InitData)
    {
        printf("iniparser_getint fail!\n");
        return ;
    }
    Data.VolumeData = iniparser_getint(ConfigIni,"Setting:volume_data",-1);
    if(-1 == Data.VolumeData)
    {
        printf("iniparser_getint fail!\n");
        return ;
    }
    Data.LanguageVersion = iniparser_getint(ConfigIni,"Setting:language_version",-1);
    if(-1 == Data.LanguageVersion)
    {
        printf("iniparser_getint fail!\n");
        return ;
    }
    
    printf("Data.InitData:%d\n", Data.InitData);
    printf("Data.VolumeData:%d\n", Data.VolumeData);
    printf("Data.LanguageVersion:%d\n", Data.LanguageVersion);
    
    /* 3. Set modify one of the parameters*/
    iniparser_set(ConfigIni,"Setting:init_data", "111");
    iniparser_set(ConfigIni,"Setting:volume_data", "222");
    iniparser_set(ConfigIni,"Setting:language_version", "333");
    
    putchar('\n');
    
    /*4. Write data to int config file*/
    DefaultIni = fopen(CONFIG_NAME, "w");
    if(NULL == DefaultIni)
    {
        printf("DefaultIni is NULL!\n");
        return ;
    }
    iniparser_dump_ini(ConfigIni, DefaultIni);
    Ret = fclose(DefaultIni);
    if(Ret != 0)
    {
        printf("close DefaultIni fail!\n");
        return ;
    }
    
    /*5.Read the modified Config ini file data*/
    Data.InitData = iniparser_getint(ConfigIni, "Setting:init_data", -1);
    if(-1 == Data.InitData)
    {
        printf("iniparser_getint fail!\n");
        return ;
    }
    Data.VolumeData = iniparser_getint(ConfigIni, "Setting:volume_data", -1);
    if(-1 == Data.VolumeData)
    {
        printf("iniparser_getint fail!\n");
        return ;
    }
    Data.LanguageVersion = iniparser_getint(ConfigIni, "Setting:language_version", -1);
    if(-1 == Data.LanguageVersion)
    {
        printf("iniparser_getint fail!\n");
        return ;
    }
    
    printf("Data.InitData:%d\n", Data.InitData);
    printf("Data.VolumeData:%d\n", Data.VolumeData);
    printf("Data.LanguageVersion:%d\n", Data.LanguageVersion);
    
    iniparser_freedict(ConfigIni);
    
    while(1)
    {
        printf("Ini test success!\n");
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
