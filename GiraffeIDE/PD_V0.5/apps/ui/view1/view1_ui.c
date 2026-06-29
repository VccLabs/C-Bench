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


static void image_button0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
	(void)ctrl; (void)event;   /* output toggle moved to label7 (ID11) */
}

static void arc0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void mbox0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void image_button1_event(grf_ctrl_t *ctrl, grf_event_e event)
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


static void label6_event(grf_ctrl_t *ctrl, grf_event_e event)
{
	switch (event) {
	    case GRF_EVENT_PRESSED:                                   /* held down */
	        grf_label_set_txt_color(ctrl, GRF_COLOR_GET(0xFF, 0xE6, 0x80)); /* light yellow */
	        break;

	    case GRF_EVENT_PRESS_LOST:                                /* dragged off, cancel */
	        grf_label_set_txt_color(ctrl, GRF_COLOR_GET(0xFF, 0x9F, 0x0A)); /* orange */
	        break;

	    case GRF_EVENT_CLICKED:                                   /* released on label -> act */
	        grf_label_set_txt_color(ctrl, GRF_COLOR_GET(0xFF, 0x9F, 0x0A)); /* orange */
	        grf_view_set_dis_view_anim(GRF_VIEW2_ID,
	            GRF_SCR_LOAD_ANIM_MOVE_LEFT, 250, 0, GRF_ANIM_PATH_END_SLOW);
	        break;

	    default: break;
	    }
}


static void image0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label7_event(grf_ctrl_t *ctrl, grf_event_e event)   /* output toggle (ID11) */
{
	if (event == GRF_EVENT_CLICKED) view1_toggle_output();
}


static void label10_event(grf_ctrl_t *ctrl, grf_event_e event)  /* nav -> view4 Settings */
{
	if (event == GRF_EVENT_CLICKED)
	    grf_view_set_dis_view_anim(GRF_VIEW4_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
}


static void label9_event(grf_ctrl_t *ctrl, grf_event_e event)   /* nav -> view3 Battery */
{
	if (event == GRF_EVENT_CLICKED)
	    grf_view_set_dis_view_anim(GRF_VIEW3_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
}


static void label8_event(grf_ctrl_t *ctrl, grf_event_e event)   /* nav -> view2 Profiles */
{
	if (event == GRF_EVENT_CLICKED)
	    grf_view_set_dis_view_anim(GRF_VIEW2_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
}


static void label11_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label13_event(grf_ctrl_t *ctrl, grf_event_e event)   /* session reset (ID16) */
{
	switch (event) {
		case GRF_EVENT_PRESSED:     view1_reset_press(1); break;  /* finger down -> tint */
		case GRF_EVENT_RELEASED:
		case GRF_EVENT_PRESS_LOST:  view1_reset_press(0); break;  /* up / slid off -> clear */
		case GRF_EVENT_CLICKED:     view1_reset_session();  break;/* completed tap -> reset */
		default: break;
	}
}


static void label12_event(grf_ctrl_t *ctrl, grf_event_e event)
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


static void label15_event(grf_ctrl_t *ctrl, grf_event_e event)
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


static void label17_event(grf_ctrl_t *ctrl, grf_event_e event)
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


static void label18_event(grf_ctrl_t *ctrl, grf_event_e event)
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


static void label20_event(grf_ctrl_t *ctrl, grf_event_e event)
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


static void label24_event(grf_ctrl_t *ctrl, grf_event_e event)   /* theme toggle (ID28) */
{
	if (event == GRF_EVENT_CLICKED) view1_toggle_theme();
}

#include "../../../libs/appscc/view1_cc.h"
void view1_init(void)
{
	grf_view_create(GRF_VIEW1_ID, view_ctrls_fun_t,sizeof(view_ctrls_fun_t) / sizeof(grf_ctrl_fun_t));
}

void view1_entry(void) 
{
	view1_sync_armed();
	view4_request_settings();   /* HMI is up -> pull saved settings into the shadow */
}

void view1_exit(void)
{
	
}
