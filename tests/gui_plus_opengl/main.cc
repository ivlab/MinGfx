/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

#include <mingfx.h>
#include "gui_plus_opengl.h"


int main(int argc, char **argv) {

    GuiPlusOpenGL *app = new GuiPlusOpenGL();
    app->Run();

    return 0;
}

