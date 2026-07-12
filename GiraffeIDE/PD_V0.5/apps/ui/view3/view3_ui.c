#include "../../apps.h"
extern u16 g_prev_view;



static void image0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label3_event(grf_ctrl_t *ctrl, grf_event_e event)   /* redirect -> Pin Map (view6) */
{
	if (event == GRF_EVENT_CLICKED)
		{
		    g_prev_view = GRF_VIEW3_ID;
		    grf_view_set_dis_view_anim(GRF_VIEW6_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
		}
	}

static void label2_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label1_event(grf_ctrl_t *ctrl, grf_event_e event)   /* nav -> view2 Profiles */
{
	if (event == GRF_EVENT_CLICKED)
	    grf_view_set_dis_view_anim(GRF_VIEW2_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
}


static void label0_event(grf_ctrl_t *ctrl, grf_event_e event)   /* nav -> view1 Monitor */
{
	if (event == GRF_EVENT_CLICKED)
	    grf_view_set_dis_view_anim(GRF_VIEW1_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
}


static void container0_event(grf_ctrl_t *ctrl, grf_event_e event)
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


static void label6_event(grf_ctrl_t *ctrl, grf_event_e event)
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


static void label8_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label11_event(grf_ctrl_t *ctrl, grf_event_e event)   /* nav -> Profiles (view2) */
{
	if (event == GRF_EVENT_CLICKED)
		grf_view_set_dis_view_anim(GRF_VIEW2_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
}


static void label10_event(grf_ctrl_t *ctrl, grf_event_e event)   /* nav -> Monitor (view1) */
{
	if (event == GRF_EVENT_CLICKED)
		grf_view_set_dis_view_anim(GRF_VIEW1_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
}


static void label9_event(grf_ctrl_t *ctrl, grf_event_e event)   /* nav -> Settings (view4) */
{
	if (event == GRF_EVENT_CLICKED)
		grf_view_set_dis_view_anim(GRF_VIEW4_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
}


static void arc0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label12_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


extern void view1_toggle_theme(void);
static void image1_event(grf_ctrl_t *ctrl, grf_event_e event)
{
	if (event == GRF_EVENT_CLICKED) view1_toggle_theme();
}


static void image2_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void image3_event(grf_ctrl_t *ctrl, grf_event_e event)
{
	if (event == GRF_EVENT_CLICKED) ocp_popups_set(0); /* tap dismisses all OCP popups */
}

#include "../../../libs/appscc/view3_cc.h"
void view3_init(void)
{
	grf_view_create(GRF_VIEW3_ID,view_ctrls_fun_t,sizeof(view_ctrls_fun_t)/sizeof(grf_ctrl_fun_t));
}

extern void view3_apply_theme(void);

extern u8 g_pinbtn;
extern void view3_tele_apply(void);
void view3_entry(void)
{
	view3_apply_theme();
	    view3_tele_apply();   /* repaint SoC/cell/state from shadow (kills default-text flash) */
	    grf_ctrl_set_hidden(GCL(GRF_VIEW3_ID, 17), g_pinbtn ? 0 : 1); /* pin-map btn gated by sw2 */
}

void view3_exit(void)
{

}
