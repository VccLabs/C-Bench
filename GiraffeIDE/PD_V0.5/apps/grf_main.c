#include "apps.h"
#include "../libs/appscc/grf_prj_cc.h"
#include "hardware/uart/grf_hw_uart.h"

static void rx_apply_task(grf_task_t *task_t) {
    char buf[64];
    if (hmi_take_rx(buf, sizeof(buf))) {
        grf_ctrl_t *lbl = grf_ctrl_get_form_id(GRF_VIEW1_ID, VIEW1_LABEL0_ID);
        grf_label_set_txt(lbl, buf);
    }
}

void grf_main(void) {
    grf_task_create(rx_apply_task, 50, NULL);
}
