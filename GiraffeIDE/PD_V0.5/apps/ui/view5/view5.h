#ifndef __VIEW5_H_
#define __VIEW5_H_

typedef enum {
	VIEW5_NULL,
	VIEW5_LABEL3_ID = 6,
	VIEW5_LABEL2_ID = 5,
	VIEW5_LABEL0_ID = 3,
	VIEW5_TXTBOX0_ID = 1,
	VIEW5_LABEL1_ID = 4,
	VIEW5_LABEL4_ID = 7,
	VIEW5_IMAGE1_ID = 9,
	VIEW5_KEYBOARD0_ID = 2
} view5_ctrls_id_e;

void view5_init(void);
void view5_entry(void);
void view5_exit(void);

#endif
