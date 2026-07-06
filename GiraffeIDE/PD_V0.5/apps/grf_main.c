#include "apps.h"
#include "../libs/appscc/grf_prj_cc.h"

void grf_main(void)
{
	extern void theme_load_boot(void);
		extern void giften_load_boot(void);
			theme_load_boot();
			giften_load_boot();
		grf_prj_create(grf_views_fun, sizeof(grf_views_fun) / sizeof(grf_view_fun_t));
		grf_hw_init();
}
