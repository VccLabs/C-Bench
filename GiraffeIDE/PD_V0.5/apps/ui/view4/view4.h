#ifndef __VIEW4_H_
#define __VIEW4_H_

typedef enum {
	VIEW4_NULL,
	VIEW4_LABEL10_ID = 12,
	VIEW4_CONTAINER0_ID = 1,
	VIEW4_LABEL6_ID = 8,
	VIEW4_LABEL5_ID = 7,
	VIEW4_LABEL4_ID = 6,
	VIEW4_LABEL3_ID = 5,
	VIEW4_LABEL0_ID = 2,
	VIEW4_LABEL1_ID = 3,
	VIEW4_LABEL2_ID = 4,
	VIEW4_LABEL8_ID = 10,
	VIEW4_SW0_ID = 13,
	VIEW4_LABEL7_ID = 9,
	VIEW4_LABEL9_ID = 11
} view4_ctrls_id_e;

void view4_init(void);
void view4_entry(void);
void view4_exit(void);

#endif
