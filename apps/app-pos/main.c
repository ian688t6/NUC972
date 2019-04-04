#include "os.h"
#include "dbg.h"
#include "basic.h"

#ifdef __cplusplus
extern "C" {
#endif

static timer_attr_s gst_timer;

static void timer_task(void *pv_arg)
{
	logi("timer ...");
	return;		
}

int main(int argc, char *argv[])
{
	logi("rf-pos");

	gst_timer.pf_cb = timer_task;
	gst_timer.pv_arg = NULL;
	if (0 != timer_setup(&gst_timer, 1, 0)) {
		loge("timer setup failed!");
		return -1;
	}

	for (;;) {
		sleep(1);
	}
	return 0;		
}

#ifdef __cplusplus
}
#endif

