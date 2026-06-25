#ifndef __GRF_HW_UART_H_
#define __GRF_HW_UART_H_








void grf_uart_init(void);
s32 grf_reg_set(u16 addr,u16 data);
s32 grf_reg_get(u16 addr);
s32 grf_reg_com_send(u16 addr,u16 len);
void select_row_by_bg(grf_ctrl_t *ctrl);
void view2_slider_changed(u8 which);   /* 0 = V slider, 1 = C slider */
void view2_use_apply(void);            /* Use button: send 0x0020/21/23, go Monitor */
void view2_reset_panel(void);
void view1_sync_armed(void);



#endif
