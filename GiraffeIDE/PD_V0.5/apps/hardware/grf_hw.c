#include "grf_hw.h"

extern void theme_load_boot(void);
void grf_hw_init(void)
{
    theme_load_boot();
    grf_uart_init();
    // grf_wdt_task_create();
}








