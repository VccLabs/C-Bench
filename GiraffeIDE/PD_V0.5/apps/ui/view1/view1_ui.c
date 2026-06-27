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
    if (event == GRF_EVENT_VALUE_CHANGED) {
        u16 on = (grf_ctrl_get_state(ctrl) & GRF_STATE_CHECKED) ? 1 : 0;
        grf_reg_set(0x0022, on);
        grf_reg_com_send(0x0022, 1);
    }
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


static void label7_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void label10_event(grf_ctrl_t *ctrl, grf_event_e event)  /* nav -> view4 Settings */
{
	if (event == GRF_EVENT_CLICKED)
	    grf_view_set_dis_view_anim(GRF_VIEW4_ID, GRF_SCR_LOAD_ANIM_MOVE_LEFT, 250, 0, GRF_ANIM_PATH_END_SLOW);
}


static void label9_event(grf_ctrl_t *ctrl, grf_event_e event)   /* nav -> view3 Battery */
{
	if (event == GRF_EVENT_CLICKED)
	    grf_view_set_dis_view_anim(GRF_VIEW3_ID, GRF_SCR_LOAD_ANIM_MOVE_LEFT, 250, 0, GRF_ANIM_PATH_END_SLOW);
}


static void label8_event(grf_ctrl_t *ctrl, grf_event_e event)   /* nav -> view2 Profiles */
{
	if (event == GRF_EVENT_CLICKED)
	    grf_view_set_dis_view_anim(GRF_VIEW2_ID, GRF_SCR_LOAD_ANIM_MOVE_LEFT, 250, 0, GRF_ANIM_PATH_END_SLOW);
}

#include "../../../libs/appscc/view1_cc.h"
void view1_init(void)
{
	grf_view_create(GRF_VIEW1_ID, view_ctrls_fun_t,sizeof(view_ctrls_fun_t) / sizeof(grf_ctrl_fun_t));
}

void view1_entry(void) 
{
	view1_sync_armed();
}

void view1_exit(void)
{
	
}
