/**
 * @file lv_ex_conf.h
 *
 */
/*
 * COPY THIS FILE AS lv_ex_conf.h
 */

#if 0 /*Set it to "1" to enable the content*/

#ifndef LV_EX_CONF_H
#define LV_EX_CONF_H

/*******************
 * GENERAL SETTING
 *******************/
#define LV_EX_PRINTF       0       /*Enable printf-ing data*/
#define LV_EX_KEYBOARD     0       /*Add PC keyboard support to some examples (`lv_drivers` repository is required)*/
#define LV_EX_MOUSEWHEEL   0       /*Add 'encoder' (mouse wheel) support to some examples (`lv_drivers` repository is required)*/

/*******************
 *   TEST USAGE
 *******************/
#define LV_USE_TESTS        0

/*******************
 * TUTORIAL USAGE
 *******************/
#define LV_USE_TUTORIALS   0


/*********************
 * APPLICATION USAGE
 *********************/

/* Test the graphical performance of your MCU
 * with different settings*/
#define LV_USE_BENCHMARK   0

/*A demo application with Keyboard, Text area, List and Chart
 * placed on Tab view */
#define LV_USE_DEMO        0
#if LV_USE_DEMO
#define LV_DEMO_WALLPAPER  1    /*Create a wallpaper too*/
#define LV_DEMO_SLIDE_SHOW 0    /*Automatically switch between tabs*/
#endif

/*MCU and memory usage monitoring*/
#define LV_USE_SYSMON      0

/*A terminal to display received characters*/
#define LV_USE_TERMINAL    0

/*Touch pad calibration with 4 points*/
#define LV_USE_TPCAL       0

#endif /*LV_EX_CONF_H*/

#endif /*End of "Content enable"*/

