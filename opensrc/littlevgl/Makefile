TARGET_TYPE := so
TARGET 		:= liblvgl.so

LOCAL_INC := -I$(OPENSRCDIR)/littlevgl 

LOCAL_INC += -Ilv_drivers/display 
LOCAL_SRC	:= lv_drivers/display/fbdev.c
LOCAL_SRC	+= lv_drivers/display/monitor.c
LOCAL_SRC	+= lv_drivers/display/R61581.c
LOCAL_SRC	+= lv_drivers/display/SSD1963.c
LOCAL_SRC	+= lv_drivers/display/ST7565.c

LOCAL_INC += -Ilv_drivers/indev 
LOCAL_SRC	+= lv_drivers/indev/FT5406EE8.c
LOCAL_SRC	+= lv_drivers/indev/keyboard.c
LOCAL_SRC	+= lv_drivers/indev/mouse.c
LOCAL_SRC	+= lv_drivers/indev/mousewheel.c
LOCAL_SRC	+= lv_drivers/indev/evdev.c
LOCAL_SRC	+= lv_drivers/indev/XPT2046.c
CCFLAGS += -DUSE_FBDEV=1

LOCAL_INC += -Ilvgl/lv_core 
LOCAL_SRC	+= lvgl/lv_core/lv_group.c
LOCAL_SRC	+= lvgl/lv_core/lv_indev.c
LOCAL_SRC	+= lvgl/lv_core/lv_obj.c
LOCAL_SRC	+= lvgl/lv_core/lv_refr.c
LOCAL_SRC	+= lvgl/lv_core/lv_style.c
LOCAL_SRC	+= lvgl/lv_core/lv_vdb.c
LOCAL_SRC	+= lvgl/lv_core/lv_lang.c

LOCAL_INC += -Ilvgl/lv_hal 
LOCAL_SRC	+= lvgl/lv_hal/lv_hal_disp.c
LOCAL_SRC	+= lvgl/lv_hal/lv_hal_indev.c
LOCAL_SRC	+= lvgl/lv_hal/lv_hal_tick.c

LOCAL_INC += -Ilvgl/lv_objx 
LOCAL_SRC	+= lvgl/lv_objx/lv_arc.c
LOCAL_SRC	+= lvgl/lv_objx/lv_bar.c
LOCAL_SRC	+= lvgl/lv_objx/lv_cb.c
LOCAL_SRC	+= lvgl/lv_objx/lv_ddlist.c
LOCAL_SRC	+= lvgl/lv_objx/lv_kb.c
LOCAL_SRC	+= lvgl/lv_objx/lv_line.c
LOCAL_SRC	+= lvgl/lv_objx/lv_mbox.c
LOCAL_SRC	+= lvgl/lv_objx/lv_preload.c
LOCAL_SRC	+= lvgl/lv_objx/lv_roller.c
LOCAL_SRC	+= lvgl/lv_objx/lv_table.c
LOCAL_SRC	+= lvgl/lv_objx/lv_tabview.c
LOCAL_SRC	+= lvgl/lv_objx/lv_tileview.c
LOCAL_SRC	+= lvgl/lv_objx/lv_btn.c
LOCAL_SRC	+= lvgl/lv_objx/lv_calendar.c
LOCAL_SRC	+= lvgl/lv_objx/lv_chart.c
LOCAL_SRC	+= lvgl/lv_objx/lv_canvas.c
LOCAL_SRC	+= lvgl/lv_objx/lv_gauge.c
LOCAL_SRC	+= lvgl/lv_objx/lv_label.c
LOCAL_SRC	+= lvgl/lv_objx/lv_list.c
LOCAL_SRC	+= lvgl/lv_objx/lv_slider.c
LOCAL_SRC	+= lvgl/lv_objx/lv_ta.c
LOCAL_SRC	+= lvgl/lv_objx/lv_spinbox.c
LOCAL_SRC	+= lvgl/lv_objx/lv_btnm.c
LOCAL_SRC	+= lvgl/lv_objx/lv_cont.c
LOCAL_SRC	+= lvgl/lv_objx/lv_img.c
LOCAL_SRC	+= lvgl/lv_objx/lv_imgbtn.c
LOCAL_SRC	+= lvgl/lv_objx/lv_led.c
LOCAL_SRC	+= lvgl/lv_objx/lv_lmeter.c
LOCAL_SRC	+= lvgl/lv_objx/lv_page.c
LOCAL_SRC	+= lvgl/lv_objx/lv_sw.c
LOCAL_SRC	+= lvgl/lv_objx/lv_win.c

LOCAL_INC += -Ilvgl/lv_fonts 
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_builtin.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_dejavu_10.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_dejavu_20.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_dejavu_30.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_dejavu_40.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_dejavu_10_cyrillic.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_dejavu_20_cyrillic.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_dejavu_30_cyrillic.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_dejavu_40_cyrillic.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_dejavu_10_latin_sup.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_dejavu_20_latin_sup.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_dejavu_30_latin_sup.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_dejavu_40_latin_sup.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_symbol_10.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_symbol_20.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_symbol_30.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_symbol_40.c
LOCAL_SRC	+= lvgl/lv_fonts/lv_font_monospace_8.c

LOCAL_INC += -Ilvgl/lv_misc 
LOCAL_SRC	+= lvgl/lv_misc/lv_font.c
LOCAL_SRC	+= lvgl/lv_misc/lv_circ.c
LOCAL_SRC	+= lvgl/lv_misc/lv_area.c
LOCAL_SRC	+= lvgl/lv_misc/lv_task.c
LOCAL_SRC	+= lvgl/lv_misc/lv_fs.c
LOCAL_SRC	+= lvgl/lv_misc/lv_anim.c
LOCAL_SRC	+= lvgl/lv_misc/lv_mem.c
LOCAL_SRC	+= lvgl/lv_misc/lv_ll.c
LOCAL_SRC	+= lvgl/lv_misc/lv_color.c
LOCAL_SRC	+= lvgl/lv_misc/lv_txt.c
LOCAL_SRC	+= lvgl/lv_misc/lv_ufs.c
LOCAL_SRC	+= lvgl/lv_misc/lv_math.c
LOCAL_SRC	+= lvgl/lv_misc/lv_log.c
LOCAL_SRC	+= lvgl/lv_misc/lv_gc.c

LOCAL_INC += -Ilvgl/lv_themes 
LOCAL_SRC	+= lvgl/lv_themes/lv_theme_alien.c
LOCAL_SRC	+= lvgl/lv_themes/lv_theme.c
LOCAL_SRC	+= lvgl/lv_themes/lv_theme_default.c
LOCAL_SRC	+= lvgl/lv_themes/lv_theme_night.c
LOCAL_SRC	+= lvgl/lv_themes/lv_theme_templ.c
LOCAL_SRC	+= lvgl/lv_themes/lv_theme_zen.c
LOCAL_SRC	+= lvgl/lv_themes/lv_theme_material.c
LOCAL_SRC	+= lvgl/lv_themes/lv_theme_nemo.c
LOCAL_SRC	+= lvgl/lv_themes/lv_theme_mono.c

LOCAL_INC += -Ilvgl/lv_draw
LOCAL_SRC	+= lvgl/lv_draw/lv_draw_vbasic.c
LOCAL_SRC	+= lvgl/lv_draw/lv_draw_rbasic.c
LOCAL_SRC	+= lvgl/lv_draw/lv_draw.c
LOCAL_SRC	+= lvgl/lv_draw/lv_draw_rect.c
LOCAL_SRC	+= lvgl/lv_draw/lv_draw_label.c
LOCAL_SRC	+= lvgl/lv_draw/lv_draw_line.c
LOCAL_SRC	+= lvgl/lv_draw/lv_draw_img.c
LOCAL_SRC	+= lvgl/lv_draw/lv_draw_arc.c
LOCAL_SRC	+= lvgl/lv_draw/lv_draw_triangle.c

CCFLAGS += -DUSE_LV_LOG

include $(TOPDIR)/rule.mk

