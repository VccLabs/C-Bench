#include "../../apps.h"



static void button0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}

#include "../../../libs/appscc/view2_cc.h"
void view2_init(void)
{
	grf_view_create(GRF_VIEW2_ID,view_ctrls_fun_t,sizeof(view_ctrls_fun_t)/sizeof(grf_ctrl_fun_t));
}

void view2_entry(void)
{

}

void view2_exit(void)
{

}
