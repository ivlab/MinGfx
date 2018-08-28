/*
 This file is part of the MinGfx Project.
 
 Copyright (c) 2017,2018 Regents of the University of Minnesota.
 All Rights Reserved.
 
 Original Author(s) of this File:
	Dan Keefe, 2018, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */

// The GraphicsApp base class can also run as its own app, but it just displays a blank screen

#include <mingfx.h>
using namespace mingfx;

int main(int argc, char **argv) {

    GraphicsApp *app = new GraphicsApp(1024, 768, "A Blank Window!");
    app->Run();

    return 0;
}
