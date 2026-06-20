#include "../grf_hw.h"
#if GRF_HW_ENABLE
#include "grf_hw_uart.h"

static grf_drv_t *drv_uart = NULL;

void grf_reg_set_user(u16 addr,u16* data,u8 datalen)
{
    if(addr == 0x0001){
    	grf_ctrl_t* lbl = GCL(GRF_VIEW1_ID, 1);   // <- your label's Control ID
        if(lbl) grf_label_set_txt(lbl, data[0] ? "hello" : "hi");
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



