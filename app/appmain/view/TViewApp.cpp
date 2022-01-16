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
	TViewMainWindow* appMainWnd = TViewMainWindow::getInstance();

	if(!appMainWnd->init())
	{
		LOG(ERROR) << "TViewMainWindow init failure!" << endl;
		return -1;
	}

	view_lvgl_process();

	return 0;
}



