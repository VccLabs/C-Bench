#ifndef __VIEW1_H_
#define __VIEW1_H_

typedef enum {
	VIEW1_NULL,
	VIEW1_LABEL2_ID = 3,
	VIEW1_LABEL3_ID = 4,
	VIEW1_LABEL0_ID = 1,
	VIEW1_LABEL1_ID = 2,
	VIEW1_LABEL5_ID = 6,
	VIEW1_LABEL4_ID = 5,
	VIEW1_SW0_ID = 7
} view1_ctrls_id_e;

void view1_init(void);
void view1_entry(void);
void view1_exit(void);

#endif
