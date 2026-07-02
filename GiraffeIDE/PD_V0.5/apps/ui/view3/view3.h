#ifndef __VIEW3_H_
#define __VIEW3_H_

typedef enum {
	VIEW3_NULL,
	VIEW3_IMAGE0_ID = 1,
	VIEW3_LABEL8_ID = 11,
	VIEW3_LABEL11_ID = 14,
	VIEW3_LABEL9_ID = 12,
	VIEW3_LABEL6_ID = 9,
	VIEW3_LABEL7_ID = 10,
	VIEW3_CONTAINER0_ID = 4,
	VIEW3_LABEL3_ID = 5,
	VIEW3_LABEL2_ID = 6,
	VIEW3_LABEL4_ID = 7,
	VIEW3_LABEL0_ID = 2,
	VIEW3_LABEL5_ID = 8,
	VIEW3_LABEL1_ID = 3,
	VIEW3_LABEL10_ID = 13
} view3_ctrls_id_e;

void view3_init(void);
void view3_entry(void);
void view3_exit(void);

#endif
