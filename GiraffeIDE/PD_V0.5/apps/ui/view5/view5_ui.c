#include "../../apps.h"


#include "../../../libs/appscc/view5_cc.h"
void view5_init(void)
{
	grf_view_create(GRF_VIEW5_ID,view_ctrls_fun_t,sizeof(view_ctrls_fun_t)/sizeof(grf_ctrl_fun_t));
}

void view5_entry(void)
{

}

void view5_exit(void)
{

}
