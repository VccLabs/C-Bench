#ifndef __VIEW2_H_
#define __VIEW2_H_

typedef enum {
	VIEW2_NULL,
	VIEW2_BUTTON0_ID = 1
} view2_ctrls_id_e;

void view2_init(void);
void view2_entry(void);
void view2_exit(void);

#endif
