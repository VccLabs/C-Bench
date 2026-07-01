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
void view2_apply_status(void);         /* view2 entry: refresh source/empty status line */
void view2_apply_theme(void);          /* view2 entry: re-apply saved theme */
void view4_set_boot_state(u8 last_used); /* view4: 0=Off,1=Last used -> reg 0x0031 */
void view4_set_autoarm(u8 on);           /* view4: 0/1 -> reg 0x0032 */
void view4_set_bright(u8 pct);           /* view4: brightness 10..100 % -> reg 0x0030 */
void view4_request_settings(void);       /* view4 entry: ask RP to push 0x0031/0x0032 */
void view4_apply_settings(void);         /* view4 entry: paint controls from shadow */
void view4_apply_theme(void);            /* view4 entry: re-apply saved theme */
void view1_sync_armed(void);
void view1_toggle_output(void);          /* view1 label7 click -> reg 0x0022 (toggle) */
void view1_toggle_output(void);          /* view1 label7 click -> reg 0x0022 (toggle) */
void view1_reset_press(u8 down);         /* view1 reset press-tint overlay show/hide   */
void view1_reset_session(void);          /* view1 ↺ click -> reg 0x0025 (trip reset)    */
void view1_toggle_theme(void);           /* TEST: dark/light text color toggle */
void view1_apply_theme(void);            /* view1 entry: re-apply saved theme */


#endif
