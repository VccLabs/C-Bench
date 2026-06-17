#include "apps.h"
#include "../libs/appscc/grf_prj_cc.h"

extern grf_drv_t *drv_uart;

// fires for EVERY widget event — we filter for our button's click
static void on_ctrl_event(s32 view_id, s32 ctrl_id, u32 event) {
    if (view_id == GRF_VIEW1_ID &&
        ctrl_id == VIEW1_BTN0_ID &&          // <-- your button's ID
        event   == GRF_EVENT_SHORT_CLICKED) {
        if (drv_uart) grf_drv_uart_send(drv_uart, (char*)"hi\n", 3);
    }
}

// runs every 50 ms; pushes any received text onto the label
static void rx_apply_task(grf_task_t *task_t) {
    if (g_rx_ready) {                         // see note below about visibility
        g_rx_ready = 0;
        grf_ctrl_t *lbl = grf_ctrl_get_form_id(GRF_VIEW1_ID, VIEW1_LABEL0_ID); // your label ID
        grf_label_set_txt(lbl, (char*)g_rx_msg);
    }
}

void grf_main(void) {
    grf_ctrl_set_event_all(on_ctrl_event);    // route widget events here
    grf_task_create(rx_apply_task, 50, NULL); // start the receive-poll task
}
