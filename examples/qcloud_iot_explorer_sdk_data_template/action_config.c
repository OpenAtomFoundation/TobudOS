#ifdef ACTION_ENABLED

#define TOTAL_ACTION_COUNTS     (1)

static TYPE_DEF_TEMPLATE_INT sg_light_blink_in_time = 0;
static TYPE_DEF_TEMPLATE_BOOL sg_light_blink_in_color = 0;
static TYPE_DEF_TEMPLATE_INT sg_light_blink_in_total_time = 0;
static DeviceProperty g_actionInput_light_blink[] = {

   {.key = "time", .data = &sg_light_blink_in_time, .type = TYPE_TEMPLATE_INT},
   {.key = "color", .data = &sg_light_blink_in_color, .type = TYPE_TEMPLATE_BOOL},
   {.key = "total_time", .data = &sg_light_blink_in_total_time, .type = TYPE_TEMPLATE_INT},
};
static TYPE_DEF_TEMPLATE_BOOL sg_light_blink_out_err_code = 0;
static DeviceProperty g_actionOutput_light_blink[] = {

   {.key = "err_code", .data = &sg_light_blink_out_err_code, .type = TYPE_TEMPLATE_BOOL},
};


static DeviceAction g_actions[]={

    {
     .pActionId = "light_blink",
     .timestamp = 0,
     .input_num = sizeof(g_actionInput_light_blink)/sizeof(g_actionInput_light_blink[0]),
     .output_num = sizeof(g_actionOutput_light_blink)/sizeof(g_actionOutput_light_blink[0]),
     .pInput = g_actionInput_light_blink,
     .pOutput = g_actionOutput_light_blink,
    },
};

#endif
