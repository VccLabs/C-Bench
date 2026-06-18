#ifndef __GRF_HW_UART_H_
#define __GRF_HW_UART_H_








void grf_uart_init(void);
s32 grf_reg_set(u16 addr,u16 data);
s32 grf_reg_get(u16 addr);
s32 grf_reg_com_send(u16 addr,u16 len);
s32 grf_hmi_send_reg(u16 addr, u16 val);   // push a reg to the RP (cmd 0x82)



#endif
