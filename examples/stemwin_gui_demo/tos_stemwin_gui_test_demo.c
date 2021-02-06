#include "cmsis_os.h"
#include "GUI.h"

#define GUI_TASK_STK_SIZE          1500
void gui_task(void *arg);
osThreadDef(gui_task, osPriorityNormal, 1, GUI_TASK_STK_SIZE);

void gui_test_demo(void)
{
    GUI_Init();
    GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();
    GUI_CURSOR_Show();
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(GUI_FONT_24B_1);
    GUI_RECT rect ;
    rect.x0 = 0 ;
    rect.y0 = 0 ;
    rect.x1 = 239 ;
    rect.y1 = rect.y0 + 24 ;
    GUI_DispStringInRectWrap("STemWin", &rect, GUI_TA_HCENTER | GUI_TA_VCENTER, GUI_WRAPMODE_NONE);

    GUI_RECT rect1 ;
    rect1.x0 = 0 ;
    rect1.y0 = 30 ;
    rect1.x1 = 239 ;
    rect1.y1 = rect1.y0 + 24 ;
    GUI_DispStringInRectWrap("Use TencentOS tiny", &rect1, GUI_TA_HCENTER | GUI_TA_VCENTER, GUI_WRAPMODE_NONE);

    GUI_RECT rect2 ;
    rect2.x0 = 0 ;
    rect2.y0 = 60 ;
    rect2.x1 = 239 ;
    rect2.y1 = rect2.y0 + 24 ;
    GUI_DispStringInRectWrap("STemWin Ver:", &rect2, GUI_TA_HCENTER | GUI_TA_VCENTER, GUI_WRAPMODE_NONE);

    GUI_RECT rect3 ;
    rect3.x0 = 0 ;
    rect3.y0 = 90 ;
    rect3.x1 = 239 ;
    rect3.y1 = rect3.y0 + 24 ;
    const char *str = GUI_GetVersionString();
    GUI_DispStringInRectWrap(str, &rect3, GUI_TA_HCENTER | GUI_TA_VCENTER, GUI_WRAPMODE_NONE);
}

void gui_task(void *arg)
{
    __HAL_RCC_CRC_CLK_ENABLE();
    gui_test_demo();

    while (1)
    {
        GUI_Delay(100);
    }
}


void application_entry(void *arg)
{
    printf("***create stemwin gui task\r\n");
    osThreadCreate(osThread(gui_task), NULL); // Create task1
}
