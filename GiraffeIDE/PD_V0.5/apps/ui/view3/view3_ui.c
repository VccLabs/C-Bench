#include "../../apps.h"


#include "../../../libs/appscc/view3_cc.h"
void view3_init(void)
{
	grf_view_create(GRF_VIEW3_ID,view_ctrls_fun_t,sizeof(view_ctrls_fun_t)/sizeof(grf_ctrl_fun_t));
}

void view3_entry(void)
{

}

void view3_exit(void)
{

}
