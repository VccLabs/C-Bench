#include "../../apps.h"



static void container0_event(grf_ctrl_t *ctrl, grf_event_e event)
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


static void label1_event(grf_ctrl_t *ctrl, grf_event_e event)
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


static void label4_event(grf_ctrl_t *ctrl, grf_event_e event)
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


static void label2_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label6_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void line0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label7_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label8_event(grf_ctrl_t *ctrl, grf_event_e event)   /* Boot output: "Off" (ID10) */
{
	if (event == GRF_EVENT_CLICKED) view4_set_boot_state(0);
}


static void label9_event(grf_ctrl_t *ctrl, grf_event_e event)   /* Boot output: "Last used" (ID11) */
{
	if (event == GRF_EVENT_CLICKED) view4_set_boot_state(1);
}


static void label10_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void sw0_event(grf_ctrl_t *ctrl, grf_event_e event)      /* Auto-arm output (ID13) */
{
	if (event == GRF_EVENT_VALUE_CHANGED) view4_set_autoarm(grf_sw_get_state(ctrl));
}


static void image0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label14_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label13_event(grf_ctrl_t *ctrl, grf_event_e event)  /* nav -> view3 Battery */
{
	if (event == GRF_EVENT_CLICKED)
	    grf_view_set_dis_view_anim(GRF_VIEW3_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
}


static void label12_event(grf_ctrl_t *ctrl, grf_event_e event)  /* nav -> view2 Profiles */
{
	if (event == GRF_EVENT_CLICKED)
	    grf_view_set_dis_view_anim(GRF_VIEW2_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
}

static void label11_event(grf_ctrl_t *ctrl, grf_event_e event)  /* nav -> view1 Monitor */
{
	if (event == GRF_EVENT_CLICKED)
	    grf_view_set_dis_view_anim(GRF_VIEW1_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
}


static void slider0_event(grf_ctrl_t *ctrl, grf_event_e event)  /* brightness (ID19) */
{
	if (event == GRF_EVENT_VALUE_CHANGED) view4_set_bright((u8)grf_slider_get_value(ctrl));
}


static void label15_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void image2_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void image1_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label16_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}

#include "../../../libs/appscc/view4_cc.h"
void view4_init(void)
{
	grf_view_create(GRF_VIEW4_ID,view_ctrls_fun_t,sizeof(view_ctrls_fun_t)/sizeof(grf_ctrl_fun_t));
}

void view4_entry(void)
{
	view4_apply_settings();   /* paint Off/Last-used + switch from last-known settings */
	grf_ctrl_set_ext_click_area(GCL(GRF_VIEW4_ID, VIEW4_LABEL8_ID), 12);  /* "Off"  bigger hit area */
	grf_ctrl_set_ext_click_area(GCL(GRF_VIEW4_ID, VIEW4_LABEL9_ID), 12);  /* "Last used"           */
		view4_request_settings();   /* pull saved Off/Last-used + auto-arm from RP */
	}

void view4_exit(void)
{

}
