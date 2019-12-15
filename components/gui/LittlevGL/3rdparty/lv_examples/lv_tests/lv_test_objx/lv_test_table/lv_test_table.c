/**
 * @file lv_test_table.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test_table.h"
#if LV_USE_TABLE && LV_USE_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create tables to test their functionalities
 */
void lv_test_table_1(void)
{
    static lv_style_t cell1_style;
    lv_style_copy(&cell1_style, &lv_style_plain);
    cell1_style.body.border.width = 1;
    cell1_style.line.width = 1;

    static lv_style_t cell_head_style;
    lv_style_copy(&cell_head_style, &lv_style_plain);
    cell_head_style.body.border.width = 1;
    cell_head_style.body.padding.top = 20;
    cell_head_style.body.padding.bottom = 20;
    cell_head_style.line.width = 1;
    cell_head_style.text.color = LV_COLOR_RED;
    cell_head_style.text.line_space = 0;
    cell_head_style.text.letter_space = 5;
    cell_head_style.text.letter_space = 3;

    /* Create a default object*/
    lv_obj_t * table1 = lv_table_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_pos(table1, 10, 10);
    lv_table_set_style(table1, LV_TABLE_STYLE_CELL1, &cell1_style);
    lv_table_set_style(table1, LV_TABLE_STYLE_CELL2, &cell_head_style);
    lv_table_set_col_cnt(table1, 4);
    lv_table_set_row_cnt(table1, 5);

    /*Set the type 1 for the first row. Thye will use */
    lv_table_set_cell_type(table1, 0, 0, 2);
    lv_table_set_cell_type(table1, 0, 1, 2);
    lv_table_set_cell_type(table1, 0, 2, 2);
    lv_table_set_cell_type(table1, 0, 3, 2);

    lv_table_set_cell_value(table1, 0, 0, "First\nnew");

    lv_table_set_cell_value(table1, 0, 1, "Very long second");

    lv_table_set_cell_value(table1, 0, 2, "Center aligned third");
    lv_table_set_cell_align(table1, 0, 2, LV_LABEL_ALIGN_CENTER);

    lv_table_set_cell_value(table1, 0, 3, "Right aligned fourth ");
    lv_table_set_cell_align(table1, 0, 3, LV_LABEL_ALIGN_RIGHT);

    lv_table_set_cell_value(table1, 2, 2, "Merge ");
    lv_table_set_cell_merge_right(table1, 2, 2, true);


    lv_table_set_cell_value(table1, 3, 1, "Vert. center");

    lv_table_set_cell_value(table1, 3, 2, "Merge center\nin three\nrows");
    lv_table_set_cell_merge_right(table1, 3, 2, true);
    lv_table_set_cell_align(table1, 3, 2, LV_LABEL_ALIGN_CENTER);

    lv_table_set_cell_value(table1, 4, 2, "Merge right");
    lv_table_set_cell_merge_right(table1, 4, 2, true);
    lv_table_set_cell_align(table1, 4, 2, LV_LABEL_ALIGN_RIGHT);


    /*Add some extra rows*/
    lv_table_set_row_cnt(table1, lv_table_get_row_cnt(table1) + 2);
    lv_table_set_cell_value(table1, 6, 0, "Multiple merge");
    lv_table_set_cell_merge_right(table1, 6, 0, true);
    lv_table_set_cell_merge_right(table1, 6, 1, true);
    lv_table_set_cell_merge_right(table1, 6, 2, true);
}

/**
 * Create tables to test their functionalities
 */
void lv_test_table_2(void)
{
    lv_obj_t * page = lv_page_create(lv_disp_get_scr_act(NULL), NULL);
    lv_page_set_style(page, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
    lv_page_set_style(page, LV_PAGE_STYLE_SCRL, &lv_style_transp_fit);
    lv_page_set_scrl_fit(page, LV_FIT_TIGHT);
    lv_obj_set_size(page, 200, 200);

    static lv_style_t cell_style;
    lv_style_copy(&cell_style, &lv_style_plain);
    cell_style.body.border.width = 1;

    /* Create a default object*/
    lv_obj_t * table1 = lv_table_create(page, NULL);
    lv_obj_set_pos(table1, 10, 10);
    lv_page_glue_obj(table1, true);
    lv_table_set_style(table1, LV_TABLE_STYLE_CELL1, &cell_style);
    lv_table_set_col_cnt(table1, 2);
    lv_table_set_row_cnt(table1, 8);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_TABLE && LV_USE_TESTS*/
