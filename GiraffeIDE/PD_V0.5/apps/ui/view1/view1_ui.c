#include "../../apps.h"

static void button0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
	// switch (event) {
	// 	case GRF_EVENT_CLICKED:
	// 		break;
	// }
}



static void label0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label5_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label3_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label1_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label4_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label2_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void sw0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
    if (event == GRF_EVENT_VALUE_CHANGED) {
        u16 on = grf_sw_get_state(ctrl) ? 1 : 0;
        grf_reg_set(0x0022, on);   // store value in ctrlreg[]
        grf_reg_com_send(0x0022, 1); // emit 5A A5 06 83 00 22 01 00 0X
    }
}


static void image_button0_event(grf_ctrl_t *ctrl, grf_event_e event)
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
