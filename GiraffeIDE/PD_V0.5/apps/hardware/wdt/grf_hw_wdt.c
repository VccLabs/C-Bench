#include "../grf_hw.h"
#if GRF_HW_ENABLE
#include "grf_hw_wdt.h"

void grf_wdt_task_cb(grf_task_t *task_t)
{
    grf_wdt_keepalive();
}

grf_task_t *wdt_task_t = NULL;
void grf_wdt_task_create(void)
{
    if(wdt_task_t==NULL){
        wdt_task_t = grf_task_create(grf_wdt_task_cb,500,NULL);
        grf_wdt_set_timeout(5);
        grf_wdt_open();
    }
}




#endif



