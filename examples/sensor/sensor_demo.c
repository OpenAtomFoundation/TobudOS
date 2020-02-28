#include "sensor_demo.h"
#include "E53_IA1.h"

void e53_ia1_demo(void)
{
    Init_E53_IA1();	
    while (1)
    {
        E53_IA1_Read_Data();
        printf("\r\n******************************BH1750 Value is  %d\r\n", (int)E53_IA1_Data.Lux);
        printf("\r\n******************************Humidity is  %d\r\n",(int)E53_IA1_Data.Humidity);
        printf("\r\n******************************Temperature is  %d\r\n", (int)E53_IA1_Data.Temperature);
        HAL_GPIO_TogglePin(IA1_Motor_GPIO_Port, IA1_Motor_Pin);  
        HAL_GPIO_TogglePin(IA1_Light_GPIO_Port, IA1_Light_Pin);           
        osDelay(3000);
    }
}

void e53_st1_demo(void)
{
    Init_E53_ST1();	
    while (1)
    {
        E53_ST1_Read_Data();
        printf("\r\n******************************Longitude Value is  %.6f\r\n", E53_ST1_Data.Longitude);
        printf("\r\n******************************Latitude is  %.6f\r\n", E53_ST1_Data.Latitude);
                
        E53_ST1_Beep(ST1_ON); 
        
        E53_ST1_LED_StatusSet(ST1_ON);
        osDelay(2000);
        E53_ST1_Beep(ST1_OFF);  
        E53_ST1_LED_StatusSet(ST1_OFF);
        osDelay(2000);

    }
}

void e53_sf1_demo(void)
{
    Init_E53_SF1();	
    while (1)
    {
        E53_SF1_Read_Data();
        printf("\r\n******************************Smoke Value is  %d\r\n", E53_SF1_Data.Smoke_Value);
        E53SF1_Beep(ON);  				        
        osDelay(2000);
        E53SF1_Beep(OFF);  
        osDelay(2000);
    }
}

void application_entry(void *arg)
{
    e53_ia1_demo();
    //e53_st1_demo();
    //e53_sf1_demo();	
    while (1) {
        printf("This is a sensor demo!\r\n");
        tos_task_delay(1000);
    }
}
