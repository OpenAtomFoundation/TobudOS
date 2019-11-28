#ifndef __SHELL_H__
#define __SHELL_H__

/** @brief Callback called when command is entered.
 *
 *  @param argc Number of parameters passed.
 *  @param argv Array of option strings. First option is always command name.
 *
 * @return 0 in case of success or negative value in case of error.
 */
typedef int (*shell_cmd_func_t)(int argc, char *argv[]);

struct shell_cmd_help {
    const char *summary;
    const char *usage;
    const struct shell_param *params;
};

struct shell_cmd {
    uint8_t sc_ext : 1; /* 1 if this is an extended shell comand. */
    shell_cmd_func_t sc_cmd_func;

    const char *sc_cmd;
    const struct shell_cmd_help *help;
};

void ble_mesh_shell_init(void);

#endif
