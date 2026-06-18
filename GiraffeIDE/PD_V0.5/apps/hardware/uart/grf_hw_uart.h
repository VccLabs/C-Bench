#ifndef __GRF_HW_UART_H_
#define __GRF_HW_UART_H_








void grf_uart_init(void);
s32 grf_reg_set(u16 addr,u16 data);
s32 grf_reg_get(u16 addr);
s32 grf_reg_com_send(u16 addr,u16 len);




#endif
