#include "../../apps.h"



static void label0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}

#include "../../../libs/appscc/view6_cc.h"
void view6_init(void)
{
	grf_view_create(GRF_VIEW6_ID,view_ctrls_fun_t,sizeof(view_ctrls_fun_t)/sizeof(grf_ctrl_fun_t));
}

void view6_entry(void)
{

}

void view6_exit(void)
{

}
