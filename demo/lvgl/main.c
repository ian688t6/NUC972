#include "os.h"
#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"

void lv_tutorial_hello_world(void)
{
    /*Create a Label on the currently active screen*/
    lv_obj_t * label1 =  lv_label_create(lv_scr_act(), NULL);

    /*Modify the Label's text*/
    lv_label_set_text(label1, "Hello world!");

    /* Align the Label to the center
     * NULL means align on parent (which is the screen now)
     * 0, 0 at the end means an x, y offset after alignment*/
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
}

int main(int argc, char *argv[])
{
    lv_disp_drv_t st_disp_drv;                         /*Descriptor of a display driver*/
	
	lv_init();

	fbdev_init();
    lv_disp_drv_init(&st_disp_drv);                    /*Basic initialization*/
    st_disp_drv.disp_flush = fbdev_flush;            /*Used in buffered mode (LV_VDB_SIZE != 0  in lv_conf.h)*/
    lv_disp_drv_register(&st_disp_drv);
	
	lv_tutorial_hello_world();
		
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

