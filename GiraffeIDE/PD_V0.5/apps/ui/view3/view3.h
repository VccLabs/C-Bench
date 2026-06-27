#ifndef __VIEW3_H_
#define __VIEW3_H_

typedef enum {
	VIEW3_NULL,
	VIEW3_IMAGE0_ID = 1,
	VIEW3_LABEL3_ID = 5,
	VIEW3_LABEL0_ID = 2,
	VIEW3_LABEL1_ID = 3
} view3_ctrls_id_e;

void view3_init(void);
void view3_entry(void);
void view3_exit(void);

#endif
