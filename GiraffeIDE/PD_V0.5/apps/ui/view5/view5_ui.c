#include "../../apps.h"



static void keyboard0_event(grf_ctrl_t *ctrl, grf_event_e event)
{

}

static void txtbox0_event(grf_ctrl_t *ctrl, grf_event_e event)
{

		if (event == GRF_EVENT_CLICKED)
			grf_ctrl_set_hidden(GCL(GRF_VIEW5_ID, VIEW5_KEYBOARD0_ID), 0);
		else if (event == GRF_EVENT_DEFOCUSED)   /* ✓ ends focus */
			grf_ctrl_set_hidden(GCL(GRF_VIEW5_ID, VIEW5_KEYBOARD0_ID), 1);
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


static void label2_event(grf_ctrl_t *ctrl, grf_event_e event)   /* Save */
{
	if (event == GRF_EVENT_CLICKED)
		{
			char *msg = grf_txtbox_get_text(GCL(GRF_VIEW5_ID, VIEW5_TXTBOX0_ID));
			if (msg)
			{
				grf_fs_file_t *f = grf_fs_open("D:/gift.txt", GRF_FS_MODE_WR);
				if (f)
				{
					u32 n = 0; while (msg[n]) n++;
					grf_fs_write(f, msg, n);
					grf_fs_close(f);
				}
			}
			grf_ctrl_set_hidden(GCL(GRF_VIEW5_ID, VIEW5_KEYBOARD0_ID), 1);
			grf_ctrl_set_hidden(GCL(GRF_VIEW5_ID, VIEW5_IMAGE1_ID), 0);
		}
}


static void label3_event(grf_ctrl_t *ctrl, grf_event_e event)
{
	switch (event) {
		case GRF_EVENT_CLICKED:{
			grf_view_set_dis_view_anim(GRF_VIEW4_ID, GRF_SCR_LOAD_ANIM_NONE, 0, 0, GRF_ANIM_PATH_END_SLOW);
		}break;
	}
}

static void label4_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void image0_event(grf_ctrl_t *ctrl, grf_event_e event)
{
//	switch (event) {
//		case GRF_EVENT_CLICKED:{
//
//		}break;
//	}
}


static void image1_event(grf_ctrl_t *ctrl, grf_event_e event)   /* saved popup */
{
	if (event == GRF_EVENT_CLICKED)
		grf_ctrl_set_hidden(GCL(GRF_VIEW5_ID, VIEW5_IMAGE1_ID), 1);
}

#include "../../../libs/appscc/view5_cc.h"
void view5_init(void)
{
	grf_view_create(GRF_VIEW5_ID,view_ctrls_fun_t,sizeof(view_ctrls_fun_t)/sizeof(grf_ctrl_fun_t));
}

void view5_entry(void)
{
	grf_keyboard_set_txtbox(GCL(GRF_VIEW5_ID, VIEW5_KEYBOARD0_ID),
	                            GCL(GRF_VIEW5_ID, VIEW5_TXTBOX0_ID));
	grf_ctrl_set_hidden(GCL(GRF_VIEW5_ID, VIEW5_KEYBOARD0_ID), 1); /* keyboard hidden at start */
		    grf_ctrl_set_hidden(GCL(GRF_VIEW5_ID, VIEW5_IMAGE1_ID), 1);    /* saved popup hidden */


		    	static char buf[256];
		    	grf_fs_file_t *f = grf_fs_open("D:/gift.txt", GRF_FS_MODE_RD);
		    	if (f)
		    	{
		    		s32 n = grf_fs_read(f, buf, sizeof(buf) - 1);
		    		grf_fs_close(f);
		    		if (n > 0)
		    		{
		    			buf[n] = 0;
		    			grf_txtbox_set_text(GCL(GRF_VIEW5_ID, VIEW5_TXTBOX0_ID), buf);
		    		}
		    	}
}


void view5_exit(void)
{

}
