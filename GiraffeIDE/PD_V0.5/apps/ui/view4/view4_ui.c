#include "../../apps.h"
extern u16 g_prev_view;



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


static void label18_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label17_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label20_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label19_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label21_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label22_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label23_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label27_event(grf_ctrl_t *ctrl, grf_event_e event)   /* Appearance: "Light" (ID34) */
{
	if (event == GRF_EVENT_CLICKED) view4_set_theme(1);
}


static void label26_event(grf_ctrl_t *ctrl, grf_event_e event)   /* Appearance: "Dark" (ID33) */
{
	if (event == GRF_EVENT_CLICKED) view4_set_theme(0);
}


static void label25_event(grf_ctrl_t *ctrl, grf_event_e event)   /* Appearance: "Light" chip (ID32) */
{
	if (event == GRF_EVENT_CLICKED) view4_set_theme(1);
}


static void label24_event(grf_ctrl_t *ctrl, grf_event_e event)   /* Appearance: "Dark" chip (ID31) */
{
	if (event == GRF_EVENT_CLICKED) view4_set_theme(0);
}


extern void view1_toggle_theme(void);
static void image3_event(grf_ctrl_t *ctrl, grf_event_e event)
{
	if (event == GRF_EVENT_CLICKED) view1_toggle_theme();
}


static void image4_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label28_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label29_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label30_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label31_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void image5_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label32_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label34_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label33_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label36_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label35_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label39_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label38_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label37_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void image6_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label40_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void image8_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void image7_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void image9_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void image12_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void image11_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void image10_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label42_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label41_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void sw1_event(grf_ctrl_t *ctrl, grf_event_e event)
{
	if (event == GRF_EVENT_VALUE_CHANGED) view4_set_giften(grf_sw_get_state(ctrl));
}


static void label43_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label44_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label45_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label47_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label46_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label48_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label49_event(grf_ctrl_t *ctrl, grf_event_e event)
{
	switch (event) {
		case GRF_EVENT_CLICKED:{
			grf_view_set_dis_view_anim(GRF_VIEW5_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
		}break;
	 }
}


static void label50_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label51_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label52_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label53_event(grf_ctrl_t *ctrl, grf_event_e event)   /* redirect -> Pin Map (view6) */
{
	if (event == GRF_EVENT_CLICKED)
	{
		g_prev_view = GRF_VIEW4_ID;
		grf_view_set_dis_view_anim(GRF_VIEW6_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
	}
}


static void label54_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label56_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label55_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label57_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label58_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label59_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


extern void view4_set_pinbtn(u8 on);
static void sw2_event(grf_ctrl_t *ctrl, grf_event_e event)       /* show/hide per-page pin-map buttons */
{
	if (event == GRF_EVENT_VALUE_CHANGED) view4_set_pinbtn(grf_sw_get_state(ctrl));
}


static void label60_event(grf_ctrl_t *ctrl, grf_event_e event)   /* redirect -> Pin Map (view6) */
{
	if (event == GRF_EVENT_CLICKED)
	{
		g_prev_view = GRF_VIEW4_ID;
		grf_view_set_dis_view_anim(GRF_VIEW6_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
	}
}


static void label61_event(grf_ctrl_t *ctrl, grf_event_e event)
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

extern u8 g_pinbtn;
void view4_entry(void)
{
	view4_apply_settings();   /* paint Off/Last-used + switch from last-known settings */
		grf_ctrl_set_hidden(GCL(GRF_VIEW4_ID, 79), g_pinbtn ? 0 : 1); /* pin-map btn gated by sw2 */
		view4_apply_theme();      /* restore dark/light colors after view reset */
	grf_ctrl_set_ext_click_area(GCL(GRF_VIEW4_ID, VIEW4_LABEL8_ID), 12);  /* "Off"  bigger hit area */
	grf_ctrl_set_ext_click_area(GCL(GRF_VIEW4_ID, VIEW4_LABEL9_ID), 12);  /* "Last used"           */
		view4_request_settings();   /* pull saved Off/Last-used + auto-arm from RP */
	}

void view4_exit(void)
{

}
