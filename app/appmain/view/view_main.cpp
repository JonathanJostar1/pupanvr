/*
 * view_main.cpp
 *
 *  Created on: 2021年12月12日
 *      Author: jhting
 */


#include "view_main.h"
#include "tlog.h"

#include "TViewApp.h"

static TView_App* gViewApp = NULL;

int 	view_init()
{
	gViewApp = new TView_App();
	LOG(INFO) << "view init ok!" << endl;
	return 0;
}

void 	view_exit()
{
	delete gViewApp;
	gViewApp = NULL;
	LOG(INFO) << "view exit!" << endl;
	return;
}


int		view_process()
{
	LOG(INFO) << "view process!" << endl;
	if(gViewApp->process() != 0)
	{
		LOG(INFO) << "viewApp test failure!" << endl;
	}

	return 0;
}

