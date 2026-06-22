#ifndef __VIEW2_H_
#define __VIEW2_H_

typedef enum {
	VIEW2_NULL,
	VIEW2_LABEL0_ID = 2,
	VIEW2_LABEL2_ID = 4,
	VIEW2_LABEL1_ID = 3,
	VIEW2_IMAGE_BUTTON0_ID = 1,
	VIEW2_IMAGE_BUTTON1_ID = 5
} view2_ctrls_id_e;

void view2_init(void);
void view2_entry(void);
void view2_exit(void);

#endif
