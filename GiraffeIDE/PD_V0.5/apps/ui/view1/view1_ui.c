#include "../../apps.h"
#include "../../hardware/uart/grf_hw_uart.h"

static void button0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
    switch (event) {
        case GRF_EVENT_SHORT_CLICKED:
            hmi_send("hi\n");
            break;
        default: break;
    }
}





static void label0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}

#include "../../../libs/appscc/view1_cc.h"
void view1_init(void)
{
	grf_view_create(GRF_VIEW1_ID, view_ctrls_fun_t,sizeof(view_ctrls_fun_t) / sizeof(grf_ctrl_fun_t));
}

void view1_entry(void) 
{

}

void view1_exit(void)
{
	
}
