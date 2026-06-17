#include "grf_hw.h"

static grf_drv_t *drv_uart = NULL;
static volatile char g_rx_msg[64] = {0};
static volatile u8   g_rx_ready  = 0;

// bytes from the RP arrive here (interrupt context — keep it short)
static void uart_rx_handler(u8 *databuf, u32 datalen) {
    u32 n = (datalen < sizeof(g_rx_msg) - 1) ? datalen : sizeof(g_rx_msg) - 1;
    memcpy((void*)g_rx_msg, databuf, n);
    while (n > 0 && (g_rx_msg[n-1] == '\n' || g_rx_msg[n-1] == '\r')) g_rx_msg[--n] = 0;
    g_rx_msg[n] = 0;
    g_rx_ready = 1;
}

void grf_uart_init(void) {
    grf_uart_cfg_t cfg = {0};
    cfg.port     = 3;            // the UART wired to your FPC/XH-4A header (default 3; set per your panel)
    cfg.win_port = 2;
    cfg.speed_e  = UART_SPEED_115200;
    cfg.bit_e    = UART_BIT_8;
    cfg.parity_e = UART_PARITY_NONE;
    cfg.stop_e   = UART_STOP_1;
    drv_uart = grf_drv_uart_open(cfg);
    if (drv_uart) grf_drv_uart_rev_set_bfun(drv_uart, uart_rx_handler, 1024);
}
