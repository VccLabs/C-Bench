#include "../../apps.h"



static void keyboard0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void txtbox0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}

#include "../../../libs/appscc/view5_cc.h"
void view5_init(void)
{
	grf_view_create(GRF_VIEW5_ID,view_ctrls_fun_t,sizeof(view_ctrls_fun_t)/sizeof(grf_ctrl_fun_t));
}

void view5_entry(void)
{
    grf_keyboard_set_txtbox(GCL(GRF_VIEW5_ID, VIEW5_KEYBOARD0_ID),
                            GCL(GRF_VIEW5_ID, VIEW5_TXTBOX0_ID));
}

void view5_exit(void)
{

}
