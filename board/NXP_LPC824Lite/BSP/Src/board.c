#include "board.h"
/* Defines ------------------------------------------------------------------*/
#define LED_GPIO        GPIO
#define LED_GPIO_PORT   0
/* Typedefs -----------------------------------------------------------------*/
/* Macros -------------------------------------------------------------------*/
/* Local variables ----------------------------------------------------------*/
static const uint8_t ledBits[BOARD_LED_CNT] = {7, 13, 16, 17, 19, 27, 28, 18};

/* Extern variables ---------------------------------------------------------*/
/* Global variables ---------------------------------------------------------*/
/* Private function prototypes ----------------------------------------------*/
static void Board_LED_Init(void)
{
    int i;

    GPIO_PortInit(LED_GPIO, LED_GPIO_PORT);
    gpio_pin_config_t config = { kGPIO_DigitalOutput, 0};
    
	for (i = 0; i < BOARD_LED_CNT; i++) {
		GPIO_PinInit(LED_GPIO, LED_GPIO_PORT, ledBits[i], &config);
		GPIO_PinWrite(LED_GPIO, LED_GPIO_PORT, ledBits[i], 1);
	}
}
/* Public functions ---------------------------------------------------------*/

/* Set the LED to the state of "On" */
void Board_LED_Set(uint8_t LEDNumber, bool On)
{
	if (LEDNumber < BOARD_LED_CNT) {
		GPIO_PinWrite(LED_GPIO, LED_GPIO_PORT, ledBits[LEDNumber], (bool)!On);
	}
}

/* Toggles the current state of a board LED */
void Board_LED_Toggle(uint8_t LEDNumber)
{
	if (LEDNumber < BOARD_LED_CNT) {
		GPIO_PortToggle(LED_GPIO, LED_GPIO_PORT, 1UL << ledBits[LEDNumber]);
	}
}


void LPC_Board_Init(void)
{
    /* Initialize the LEDs */
    Board_LED_Init();

}
/* Private functions --------------------------------------------------------*/

