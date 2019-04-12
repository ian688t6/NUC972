#include "os.h"
#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"

int main(int argc, char *argv[])
{
    lv_disp_drv_t st_disp_drv;                         /*Descriptor of a display driver*/
	
	lv_init();

	fbdev_init();
    lv_disp_drv_init(&st_disp_drv);                    /*Basic initialization*/
    st_disp_drv.disp_flush = fbdev_flush;            /*Used in buffered mode (LV_VDB_SIZE != 0  in lv_conf.h)*/
    lv_disp_drv_register(&st_disp_drv);

	
    /*************************************
     * Run the task handler of LittlevGL
     *************************************/
    while(1) {
        /* Periodically call this function.
         * The timing is not critical but should be between 1..10 ms */
   		lv_tick_inc(5);
   		lv_task_handler();
        /*delay_ms(5)*/
    }

	return 0;
}

