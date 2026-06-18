#include "../../apps.h"

static void button0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
    if (event == GRF_EVENT_CLICKED)
        grf_hmi_send_reg(0x30, 1);          // ping RP
}
static void sw_out_event(grf_ctrl_t *ctrl, grf_event_e event)
{
    if (event == GRF_EVENT_VALUE_CHANGED)
        grf_hmi_send_reg(0x22, grf_sw_get_state(ctrl) ? 1 : 0);
}



static void BATT_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void LBL_P_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void LBL_I_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void LBL_V_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
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

static grf_task_t *s_task = NULL;

static void status_task(grf_task_t *t)
{
	grf_label_set_txt(GCL(GRF_VIEW1_ID, VIEW1_LABEL0_ID),
	                      grf_reg_get(0x10) ? "hello" : "hi");
}

void view1_entry(void)
{
    s_task = grf_task_create(status_task, 200, NULL);
}

void view1_exit(void)
{
    if (s_task) { grf_task_del(s_task); s_task = NULL; }
}
