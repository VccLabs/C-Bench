#include "../../apps.h"
extern u16 g_prev_view;

static void view1_hide_boot_msg(void);

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


static void label26_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label25_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void image2_event(grf_ctrl_t *ctrl, grf_event_e event)
{
	if (event == GRF_EVENT_CLICKED) view1_toggle_theme();
}


static void image3_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void image4_event(grf_ctrl_t *ctrl, grf_event_e event)
{
	if (event == GRF_EVENT_CLICKED) view1_hide_boot_msg();
}


static void label24_event(grf_ctrl_t *ctrl, grf_event_e event)
{
	if (event == GRF_EVENT_CLICKED) view1_hide_boot_msg();
}


static void label27_event(grf_ctrl_t *ctrl, grf_event_e event)   /* redirect -> Pin Map (view6) */
{
	if (event == GRF_EVENT_CLICKED)
		{
			g_prev_view = GRF_VIEW1_ID;
			grf_view_set_dis_view_anim(GRF_VIEW6_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
		}
	}


static void image5_event(grf_ctrl_t *ctrl, grf_event_e event)
{
	if (event == GRF_EVENT_CLICKED) ocp_popups_set(0); /* tap dismisses all OCP popups */
}


static void label28_event(grf_ctrl_t *ctrl, grf_event_e event)
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


static void image7_event(grf_ctrl_t *ctrl, grf_event_e event)
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

#include "../../../libs/appscc/view1_cc.h"
void view1_init(void)
{
	grf_view_create(GRF_VIEW1_ID, view_ctrls_fun_t,sizeof(view_ctrls_fun_t) / sizeof(grf_ctrl_fun_t));
}

static u8 g_bootMsgShown = 0;
static void view1_load_boot_msg(void)
{
	static char buf[256];
	grf_fs_file_t *f = grf_fs_open("D:/gift.txt", GRF_FS_MODE_RD);
	if (!f) { grf_ctrl_set_hidden(GCL(GRF_VIEW1_ID, VIEW1_IMAGE4_ID), 1); return; }
	s32 n = grf_fs_read(f, buf, sizeof(buf) - 1);
	grf_fs_close(f);
	if (n <= 0) { grf_ctrl_set_hidden(GCL(GRF_VIEW1_ID, VIEW1_IMAGE4_ID), 1); return; }
	buf[n] = 0;
	grf_label_set_txt(GCL(GRF_VIEW1_ID, VIEW1_LABEL24_ID), buf);
	grf_ctrl_set_hidden(GCL(GRF_VIEW1_ID, VIEW1_LABEL24_ID), 0);
	grf_ctrl_set_hidden(GCL(GRF_VIEW1_ID, VIEW1_IMAGE4_ID), 0);
}

static void view1_hide_boot_msg(void)
{
	grf_ctrl_set_hidden(GCL(GRF_VIEW1_ID, VIEW1_IMAGE4_ID), 1);
	grf_ctrl_set_hidden(GCL(GRF_VIEW1_ID, VIEW1_LABEL24_ID), 1);
	/* grf_ctrl_set_hidden(GCL(GRF_VIEW1_ID, VIEW1_SCRIM_ID), 1);  // add once scrim ID known */
}

extern u8 g_pinbtn;
extern void view1_tele_apply(void);
void view1_entry(void) 
{
	view1_sync_armed();
		view1_tele_apply();       /* repaint live labels from shadow (kills default-text flash) */
		grf_ctrl_set_hidden(GCL(GRF_VIEW1_ID, 34), g_pinbtn ? 0 : 1); /* pin-map btn gated by sw2 */
	view1_sync_armed();
		view4_request_settings();   /* HMI is up -> pull saved settings into the shadow */
		view1_apply_theme();
		view1_reset_press(0);       /* force press-tint overlay hidden on entry (default-visible fix) */
		if (g_giften && !g_bootMsgShown) { g_bootMsgShown = 1; view1_load_boot_msg(); }
			else grf_ctrl_set_hidden(GCL(GRF_VIEW1_ID, VIEW1_IMAGE4_ID), 1);
}

void view1_exit(void)
{
	
}
