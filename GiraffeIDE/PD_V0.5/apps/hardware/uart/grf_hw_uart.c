#include "../grf_hw.h"
#if GRF_HW_ENABLE
#include "grf_hw_uart.h"

static grf_drv_t *drv_uart = NULL;

#define LBL_VOLT   1
#define ARC_VOLT   7   /* <- your arc's Control ID from view1.h */

#define LBL_CURR   3
#define LBL_POWER  5

#define MAX_PROF 13
typedef struct { u16 type, vmin, vmax, imax; } prof_t;
static prof_t g_prof[MAX_PROF];
static u8 g_prof_n = 0;

#define LBL_EMPTY1  80   /* label78 - "No profiles..." view2*/
#define LBL_EMPTY2  81   /* label79 - subtitle view2*/

/* view2 adjust panel + Use button (PPS/AVS fine-adjust) */
#define ADJ_CONT 82   /* container1            */
#define ADJ_SV   83   /* slider0 - set voltage  -> reg 0x0020 (mV) */
#define ADJ_SC   87   /* slider1 - current limit-> reg 0x0021 (mA) */
#define ADJ_LV   88   /* label82 - voltage value */
#define ADJ_LC   89   /* label83 - current value */
#define BTN_USE  90   /* label84 - Use/apply button */

/* per-row Control IDs: {badge, volt, meta, curr, check} */
enum { COL_BADGE, COL_VOLT, COL_META, COL_CURR, COL_CHECK, COL_BG };
static const u16 ROW_ID[MAX_PROF][6] = {
  /* {badge, volt, meta, curr, check, bg} */
  {  6,  2,  3,  4,  5, 67 },  /* row 0  */
  { 10,  9,  8,  7,  1, 68 },  /* row 1  */
  { 12, 11, 13, 14, 15, 79 },  /* row 2  */
  { 26, 27, 28, 29, 30, 78 },  /* row 3  */
  { 25, 24, 18, 22, 21, 77 },  /* row 4  */
  { 16, 17, 23, 19, 20, 76 },  /* row 5  */
  { 55, 54, 43, 42, 31, 69 },  /* row 6  */
  { 56, 53, 44, 41, 32, 70 },  /* row 7  */
  { 57, 52, 45, 40, 33, 71 },  /* row 8  */
  { 58, 51, 46, 39, 34, 72 },  /* row 9  */
  { 59, 50, 47, 38, 35, 73 },  /* row 10 */
  { 60, 49, 48, 37, 36, 75 },  /* row 11 */
  { 65, 64, 63, 62, 61, 74 },  /* row 12 */
};
static void show_row(u8 i, u8 vis)
{
	for(u8 k=0;k<6;k++)
        grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, ROW_ID[i][k]), vis?0:1);
}

static u8 g_sel = 0xFF;   /* selected row, 0xFF = none */

static void highlight_row(u8 i, u8 on)
{
    /* check mark */
    grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_CHECK]), on ? 0 : 1);
    /* background chip: orange tint when selected, default card when not */
    grf_ctrl_style_set_bg_color(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_BG]),
        on ? GRF_COLOR_GET(0x3A,0x2A,0x10) : GRF_COLOR_GET(0x1C,0x1C,0x1E), 0);
}

static void use_btn_set(u8 enabled, const char *txt)
{
    grf_label_set_txt(GCL(GRF_VIEW2_ID, BTN_USE), txt);
    grf_ctrl_style_set_bg_color(GCL(GRF_VIEW2_ID, BTN_USE),
        enabled ? GRF_COLOR_GET(0xFF,0x9F,0x0A) : GRF_COLOR_GET(0x2C,0x2C,0x2E), 0);
    grf_label_set_txt_color(GCL(GRF_VIEW2_ID, BTN_USE),
        enabled ? GRF_COLOR_GET(0x23,0x13,0x00) : GRF_COLOR_GET(0x8E,0x8E,0x93));
}

static void adj_labels(u16 mv, u16 ma)
{
    char b[16];
    snprintf(b,sizeof(b),"%u.%02u V", mv/1000, (mv%1000)/10);
    grf_label_set_txt(GCL(GRF_VIEW2_ID, ADJ_LV), b);
    snprintf(b,sizeof(b),"%u.%u A", ma/1000, (ma%1000)/100);
    grf_label_set_txt(GCL(GRF_VIEW2_ID, ADJ_LC), b);
}

static void update_adjust(u8 i)
{
    prof_t *p = &g_prof[i];
    u8 range = (p->type==1 || p->type==2) && (p->vmin != p->vmax);
    char b[20];
    if (range) {
        grf_slider_set_range(GCL(GRF_VIEW2_ID, ADJ_SV), p->vmin, p->vmax);
        grf_slider_set_value(GCL(GRF_VIEW2_ID, ADJ_SV), p->vmax);
        grf_slider_set_range(GCL(GRF_VIEW2_ID, ADJ_SC), 0, p->imax);
        grf_slider_set_value(GCL(GRF_VIEW2_ID, ADJ_SC), p->imax);
        adj_labels(p->vmax, p->imax);
        grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, ADJ_CONT), 0);   /* show panel */
        snprintf(b,sizeof(b),"Use %u.%02u V", p->vmax/1000, (p->vmax%1000)/10);
    } else {
        grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, ADJ_CONT), 1);   /* hide panel */
        snprintf(b,sizeof(b),"Use %u.%02u V", p->vmin/1000, (p->vmin%1000)/10);
    }
    use_btn_set(1, b);
}

void select_row_by_bg(grf_ctrl_t *ctrl)
{
    for (u8 i = 0; i < MAX_PROF; i++) {
        if (GCL(GRF_VIEW2_ID, ROW_ID[i][COL_BG]) == ctrl) {
            if (g_sel != 0xFF) highlight_row(g_sel, 0);   /* clear previous */
            g_sel = i;
                        highlight_row(i, 1);                          /* highlight new */
                        update_adjust(i);                             /* drive panel + Use btn */
                        return;
        }
    }
}

void view2_slider_changed(u8 which)
{
    u16 mv = grf_slider_get_value(GCL(GRF_VIEW2_ID, ADJ_SV));
    u16 ma = grf_slider_get_value(GCL(GRF_VIEW2_ID, ADJ_SC));
    char b[20];
    (void)which;
    adj_labels(mv, ma);
    snprintf(b,sizeof(b),"Use %u.%02u V", mv/1000, (mv%1000)/10);
    use_btn_set(1, b);
}

void view2_use_apply(void)
{
    prof_t *p;
    if (g_sel == 0xFF) return;                 /* nothing selected */
    p = &g_prof[g_sel];
    if ((p->type==1 || p->type==2) && (p->vmin != p->vmax)) {
        grf_reg_set(0x0020, grf_slider_get_value(GCL(GRF_VIEW2_ID, ADJ_SV)));
        grf_reg_com_send(0x0020, 1);           /* latch mV first */
        grf_reg_set(0x0021, grf_slider_get_value(GCL(GRF_VIEW2_ID, ADJ_SC)));
        grf_reg_com_send(0x0021, 1);           /* then mA */
    }
    grf_reg_set(0x0023, g_sel);
    grf_reg_com_send(0x0023, 1);               /* apply: RP maps pos -> PDO, arms */
    use_btn_set(1, "Applied");
    grf_view_set_dis_view_anim(GRF_VIEW1_ID, GRF_SCR_LOAD_ANIM_MOVE_RIGHT,
                               250, 0, GRF_ANIM_PATH_END_SLOW);
}

void view2_reset_panel(void)
{
    grf_label_set_txt(GCL(GRF_VIEW2_ID, ADJ_LV), "0.00 V");
    grf_label_set_txt(GCL(GRF_VIEW2_ID, ADJ_LC), "0.0 A");
    grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, ADJ_CONT), 1);
        use_btn_set(0, "Select a rail");
        grf_reg_set(0x0024, 1);      /* ask the RP to (re)push the PDO list now */
        grf_reg_com_send(0x0024, 1);
    }

static void fill_row(u8 i, prof_t *p)
{
    char v[20], c[16];
    const char *badge; grf_color_t bbg, btx;
    switch(p->type){
      case 1: badge="PPS"; bbg=GRF_COLOR_GET(0x64,0xD2,0xFF); btx=GRF_COLOR_GET(0x06,0x2A,0x30); break;
      case 2: badge="AVS"; bbg=GRF_COLOR_GET(0xFF,0x9F,0x0A); btx=GRF_COLOR_GET(0x2A,0x18,0x00); break;
      case 3: badge="EPR"; bbg=GRF_COLOR_GET(0xBF,0x5A,0xF2); btx=GRF_COLOR_GET(0x1E,0x0C,0x33); break;
      default:badge="FIX"; bbg=GRF_COLOR_GET(0x2C,0x2C,0x2E); btx=GRF_COLOR_GET(0x8E,0x8E,0x93); break;
    }
    u8 range = (p->vmin != p->vmax);

    if(!range) snprintf(v,sizeof(v),"%u.%02u V", p->vmin/1000, (p->vmin%1000)/10);
    else       snprintf(v,sizeof(v),"%u.%u-%u.%u V", p->vmin/1000,(p->vmin%1000)/100,
                                                     p->vmax/1000,(p->vmax%1000)/100);
    snprintf(c,sizeof(c),"%u.%u A", p->imax/1000, (p->imax%1000)/100);

    grf_label_set_txt          (GCL(GRF_VIEW2_ID, ROW_ID[i][COL_BADGE]), badge);
    grf_ctrl_style_set_bg_color(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_BADGE]), bbg, 0);
    grf_label_set_txt_color    (GCL(GRF_VIEW2_ID, ROW_ID[i][COL_BADGE]), btx);
    grf_label_set_txt(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_VOLT]), v);
    grf_label_set_txt(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_META]), range?"Adjustable rail":"Fixed rail");
    grf_label_set_txt(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_CURR]), c);
    show_row(i, 1);
}

void grf_reg_set_user(u16 addr,u16* data,u8 datalen)
{
    char buf[16];

    /* profile rows: 0x0110 + i*4 = [type, vmin, vmax, imax] */
    if(addr>=0x0110 && addr<0x0110+MAX_PROF*4 && datalen>=4){
        u8 i=(addr-0x0110)/4;
        g_prof[i].type=data[0]; g_prof[i].vmin=data[1];
        g_prof[i].vmax=data[2]; g_prof[i].imax=data[3];
        return;
    }

    switch(addr){
        case 0x0010:  /* voltage mV */
            snprintf(buf,sizeof(buf),"%u.%02u", data[0]/1000, (data[0]%1000)/10);
            grf_label_set_txt(GCL(GRF_VIEW1_ID, LBL_VOLT), buf);
            grf_arc_set_value(GCL(GRF_VIEW1_ID, ARC_VOLT), data[0]/100); /* 0..280 = 0..28.0V */
            break;
        case 0x0011:  /* current mA */
            snprintf(buf,sizeof(buf),"%u.%03u A", data[0]/1000, data[0]%1000);
            grf_label_set_txt(GCL(GRF_VIEW1_ID, LBL_CURR), buf);
            break;
        case 0x0012:  /* power dW (0.1W) */
            snprintf(buf,sizeof(buf),"%u.%u W", data[0]/10, data[0]%10);
            grf_label_set_txt(GCL(GRF_VIEW1_ID, LBL_POWER), buf);
            break;
        case 0x0101: {            /* list ready -> render */
        	g_prof_n = grf_reg_get(0x0100);
        	        	            if(g_prof_n > MAX_PROF) g_prof_n = MAX_PROF;
        	        	            g_sel = 0xFF;
        	        	            grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, ADJ_CONT), 1);
        	        	            use_btn_set(0, "Select a rail");
        	            grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, LBL_EMPTY1), g_prof_n ? 1 : 0);
        	            grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, LBL_EMPTY2), g_prof_n ? 1 : 0);
                            for(u8 i=0; i<g_prof_n; i++){
                                prof_t p;
                                p.type = grf_reg_get(0x0110 + i*4 + 0);
                                p.vmin = grf_reg_get(0x0110 + i*4 + 1);
                                p.vmax = grf_reg_get(0x0110 + i*4 + 2);
                                p.imax = grf_reg_get(0x0110 + i*4 + 3);
                                fill_row(i, &p);
                            }
                            for(u8 i=g_prof_n; i<MAX_PROF; i++) show_row(i, 0);  /* hide unused rows */
                            for(u8 i=0; i<g_prof_n; i++)
                                            grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_CHECK]), 1);
                            break;
                        }
    }
}


#define HEAD_FH 0x5A
#define HEAD_FL 0xA5
#define REG_LEN 0x800 
static u16 ctrlreg[REG_LEN] = {0};

s32 grf_reg_set(u16 addr,u16 data)
{
    if(addr>REG_LEN){
        return GRF_FAIL;
    }
    ctrlreg[addr] = data;
    // grf_printf("set reg %04X = %04X\n",addr,data);
    return GRF_OK;
}

s32 grf_reg_get(u16 addr)
{
    if(addr>REG_LEN){
        return GRF_FAIL;
    }
    // grf_printf("get reg %04X = %04X\n",addr,ctrlreg[addr]);
    return ctrlreg[addr];
}
//*********write reg********** 
//TX - FH FL len cmd addr data0 data1
//     5A A5 07  82  0001 FFFF  FFFF
//RX - 无

//*********read reg********** 
//TX -  FH FL len cmd addr reglen
//      5A A5 04  83  0001  02
//RX -  FH FL len cmd addr reglen data0 data1
//      5A A5 08  83  0001  02    FFFF  FFFF

static s32 grf_reg_s_set(u16 addr,u8* data,u8 len)
{
    if(addr>REG_LEN){
        return GRF_FAIL;
    }
    u8 i=0;
    for(i=0;i<len;i++){
        ctrlreg[addr+i] = (data[i*2]<<8)+data[i*2+1];
        // grf_printf("reg_s_set %04X = %04X\n",addr+i,ctrlreg[addr+i]);
        grf_reg_set_user(addr,ctrlreg+addr+i,1);
    }
    return GRF_OK;
}
s32 grf_reg_com_send(u16 addr,u16 len)
{
    u8 txdata[257] = {0};
    u32 i=0,j=0;
    txdata[i++] = HEAD_FH;
    txdata[i++] = HEAD_FL;
    txdata[i++] = len*2+4;
    txdata[i++] = 0x83;
    txdata[i++] = addr>>8;
    txdata[i++] = (addr&0x00ff);
    txdata[i++] = len;
    for(j=0;j<len;j++){
        txdata[i++] = (ctrlreg[addr+j]>>8);
        txdata[i++] = (ctrlreg[addr+j]&0x00ff);
    }
    grf_drv_uart_send(drv_uart,txdata,i);
}

static s32 grf_comm_handle(u8* data)
{
	u8 cmd = data[3];
	u8 len = data[2];
    u16 addr = (data[4]<<8)+data[5];
    u32 i=0;
    if(addr>REG_LEN){
        return GRF_FAIL;
    }
//    grf_printf("cmd=%x\n",cmd);
    switch (cmd)
    {
        case 0x82: //写寄存器
        {
            u32 regcount = (len-3)>>1;
            grf_reg_s_set(addr,data+6,regcount);
        }
        break;
        case 0x83: //读寄存器
        	grf_reg_com_send(addr,data[6]);
        break;
    }
    return GRF_OK;
}

#define UART_LASTBUFF 1
#if UART_LASTBUFF
#define RX_BUF_LEN   1024
static u8 RX_HAND_BUF[RX_BUF_LEN];
#endif
static void recive_data_handle(u8* databuf,u32 datalen)
{
	u16 i = 0;
	static u16 last_data_num=0;
#if UART_LASTBUFF
	if(last_data_num + datalen > RX_BUF_LEN){
		datalen = RX_BUF_LEN - last_data_num;
	}
	if(last_data_num){ 
		memcpy(RX_HAND_BUF+last_data_num,databuf,datalen);
		databuf=RX_HAND_BUF;
		datalen+=last_data_num;
		last_data_num = 0;
	}
#endif
#if 0
    u16 j;
    grf_printf("uart rx[%d]:",datalen);
	for(j = 0;j < datalen; j++){
		grf_printf(" %02X",databuf[j]);
	}
	grf_printf("\n",datalen);
#endif
	if(datalen >= 6){//最短的指令为6个
		for(i = 0;i <= datalen-6; i++)
        {
			if((databuf[i]==HEAD_FH) && (databuf[i+1]==HEAD_FL))
			{
    			if(databuf[i+2] <= (datalen-i-3)){
                    if(grf_comm_handle(databuf+i)==GRF_OK){
                        i += (databuf[i+2]+3)-1;
                    }
    			}
			}	
		}
	}
    if(i < datalen){
        last_data_num = datalen-i;
    }

#if UART_LASTBUFF	
	if(last_data_num <= 256 && last_data_num != 0)
	{
		u8  last_data_buf[256] = {0};
		memcpy(last_data_buf,databuf+i,last_data_num);
		memcpy(RX_HAND_BUF,last_data_buf,last_data_num);
	}else{
		last_data_num=0;			
	}
#else
	last_data_num = 0;
#endif
}


void grf_uart_init(void)
{
    grf_uart_cfg_t cfg_t = {0};
    cfg_t.port = 6;
    cfg_t.win_port = -1;
    cfg_t.speed_e = UART_SPEED_115200;
    cfg_t.bit_e = UART_BIT_8;
    cfg_t.parity_e = UART_PARITY_NONE;
    cfg_t.stop_e = UART_STOP_1;
    drv_uart = grf_drv_uart_open(cfg_t);
    if(drv_uart){
        grf_drv_uart_rev_set_bfun(drv_uart,recive_data_handle,1024);
    }
}


#endif



