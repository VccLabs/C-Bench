#ifndef __VIEW5_H_
#define __VIEW5_H_

typedef enum {
	VIEW5_NULL,
	VIEW5_TXTBOX0_ID = 1,
	VIEW5_KEYBOARD0_ID = 2
} view5_ctrls_id_e;

void view5_init(void);
void view5_entry(void);
void view5_exit(void);

#endif
