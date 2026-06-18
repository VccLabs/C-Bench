#ifndef __GRF_HW_H_
#define __GRF_HW_H_



#include "../../inc/grf_apis.h"
#define GRF_HW_ENABLE  1




//以下.h自动添加
#include "../apps.h"
#include "uart/grf_hw_uart.h"
#include "wdt/grf_hw_wdt.h"




void grf_hw_init(void);

#endif

