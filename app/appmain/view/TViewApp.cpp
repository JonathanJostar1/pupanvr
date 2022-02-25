/*
 * TViewApp.cpp
 *
 *  Created on: 2021年12月12日
 *      Author: jhting
 */

#include <stdio.h>
#include <string.h>

#include "view_lvgl.h"

#include "TViewMainWindow.h"
#include "TViewApp.h"
#include "tlog.h"



TView_App::TView_App()
{
	view_lvgl_init();
}

TView_App::~TView_App()
{
	view_lvgl_exit();
}


int TView_App::process()
{
#if 1

	lv_demo_widgets();

	TViewMainWindow* appMainWnd = TViewMainWindow::getInstance();

	if(!appMainWnd->init())
	{
		LOG(ERROR) << "TViewMainWindow init failure!" << endl;
		return -1;
	}
#else
	/*Create a Demo*/
	    lv_demo_widgets();
	    //lv_demo_benchmark();
	    //lv_demo_keypad_encoder();
	    //lv_demo_music();
	    //lv_demo_stress();

	    //lv_example_arc_1();

#endif


	view_lvgl_process();

	return 0;
}



