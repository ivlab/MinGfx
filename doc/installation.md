Installation Guide {#installation}
==========

# Prereqs {#install-prereqs}

MinGfx is built using the cross-platform CMake tool.  Before building MinGfx, you will need to install CMake version 3.9 or higher (https://cmake.org/). On Mac, you will need to have Xcode installed as well as the [optional Xcode command line tools](https://www.embarcadero.com/starthere/xe5/mobdevsetup/ios/en/installing_the_commandline_tools.html). 

If you haven't already, you'll want to clone the MinGfx source code using git, so make sure you have git installed as well.


# Background for CMake Beginners {#cmake-background}

If you are new to CMake, this is the minimum background you should have before building MinGfx.

CMake is a cross-platform tool for generating native Unix Makefiles and project workspaces (e.g., Xcode projects, Visual Studio solutions) for building projects.  Projects use CMakeLists.txt files to describe how they should be built.  These files provide input for the cmake application, which uses them to generate project files for a variety of development environments.  CMake can generate Makefiles for building MinGfx on linux systems or project files for building MinGfx with popular IDEs (e.g., XCode, Visual Studio, Eclipse, CodeBLocks).  

There are three versions of the cmake application that are provided with the install from Kitware.  You can use any of these with MinGfx:
1. *cmake* is the command line version where all build options must be specified on the command line.
2. *ccmake* is a visual interface that runs in a terminal.  This is nice because all of the available build options are listed for you to see at once, and you can simply press the SPACEBAR to turn them on/off or type in specific values.  The downside for ccmake is that it does not have a separate log window, so status messages flash by so fast that you cannot read them, and input from spin-off cmake processes (like those used in the MinGfx configure step) are not captured correctly.  It's possible to still use ccmake with MinGfx, but we do not recommend it.
3. *cmake-gui* is an OS-specific windowed version.  Like ccmake, the GUI provides a useful list all of the available build options.  Unlike ccmake, it also includes a nice scrollable log of all of the status messages.  So, this is the preferred version for new users and for the first time you build a project.

With CMake, it's best to do an out-of-source build.  That means that when you start CMake you must tell it two paths: 1. the path to the MinGfx source tree, AND 2. the path to the place where you would like all the built files to be created.  When running cmake-gui, these can be set interactively with a file selection dialog, but all three versions respond to command line options, and this is generally the easiest way to start up cmake.  So, we suggest starting *cmake-gui* from the command line.  First cd to the directory you wish to use for the built files, then pass a single required command line argument for the path to the MinGfx source tree.  Usually, you will see this written as a series of 3 steps -- the common idiom for cmake builds:
~~~
// from within the root of the project's source directory
mkdir build
cd build
cmake-gui ..
~~~

If you are using *cmake* you can pass a variety of other options to CMake on the command line with flags such as -D AUTOBUILD_NANOGUI=ON, to tell MinGfx to autobuild the nanogui library that it depends upon.  With *cmake*, you can end up with a long string of arguments in order to setup the build you want.  With *cmake-gui*, you set all of these build options within the user interface instead, and this is how we will proceed in the instructions that follow.


# Setup a New Graphics Build Tree {#build-tree}

Here's a complete annotated list of steps to setup a new build tree for creating graphics programs.  In truth, you don't need to create this entire build tree strucutre just to work with MinGfx, but this is good practice, and a good guide for students.


## 1. Create a Linux-Style Directory Tree

Libraries will get installed to a lib/ directory.  Include files will go in include/, executables in bin/, and we'll put the source we are developing inside dev/.

~~~
mkdir -p /Users/keefe/courses/csci4611/sw
cd /Users/courses/csci4611/sw
mkdir lib
mkdir include
mkdir bin
mkdir dev
~~~

IMPORTANT:  On Windows, you will need to add the `bin` directory you just created to your PATH environment variable.  If you have not done this before, you will find many instructions online.  Here is one good description of [how to add a directory to the PATH environment variable on Windows](https://www.java.com/en/download/help/path.xml).



## 2. Download the MinGfx Project
To download use:
~~~
cd dev
git clone http://github.com/ivlab/MinGfx
~~~


## 3. Create an Initial CMake Build Configuration

~~~
cd MinGfx
mkdir build
cd build
cmake-gui ..
~~~

Press the 'Configure' button. Then, select the Generator you would like to use from the list provided (e.g., Xcode, Visual Studio, Unix Makefiles).  Click Done.

Wait for CMake to do an initial configuration.  This will populate an initial list of CMake and MinGfx build options.  Each time you run configure, *cmake-gui* will display any new configuration options highlighted in *red*.  So, do not be alarmed when the whole options window turns red during this first configure operation -- that is normal behavior since at this point, all of the options are new.

Do you see an error in the CMake output window?  If so, don't worry about this yet, we'll come back to it in a minute.


## 4. Set the CMAKE_INSTALL_PREFIX Variable to the Root of Your Directory Tree

Now, using cmake-gui, scroll through the table of variables until you find the one named CMAKE_INSTALL_PREFIX.  Click on it and edit its value to point to the root of the directory tree you created in Step 1.  The root for the example tree used in Step 1 would be /Users/keefe/courses/csci4611/sw.  In other words, the root is the last directory before you get to lib, include, bin, and dev.

Once you make this change, you can press the Configure button again.  It doesn't hurt to press Configure multiple times.  You need to do it at least once after every change you make.  Notice how the variables highlighted in red go away when you do this because they are no longer new options -- cmake knows that you have already had a chance to see them once.


## 5. If Needed, Set AUTOBUILD_NANOGUI to ON and Build Dependencies

Now, take a look at the output window at the bottom of cmake-gui.  If you see the following error there, then we'll address that next.  If not, then nanogui must already be installed on your system, and you can skip to step 6.
~~~
Searching for NanoGUI library...
Could NOT find NANOGUI (missing: NANOGUI_LIBRARIES)
CMake Error at cmake/UseNanoGUI.cmake:96 (message):
The NanoGUI library was not found on the system.  You can: (1) install
NanoGUI yourself, (2) point cmake to an already-installed version of
NanoGUI by adding the installation prefix of NanoGUI to the
CMAKE_PREFIX_PATH environment variable, or (3) set AUTOBUILD_NANOGUI to ON
and run cmake configure again.  AUTOBUILD_NANOGUI will: 1.  download
NanoGUI, 2.  build NanoGUI, and 3.  install NanoGUI to the
CMAKE_INSTALL_PREFIX (currently set to
/Users/keefe/courses/tmp/MinGfx/build/install).  If you want to use the
AUTOBUILD feature, adjust CMAKE_INSTALL_PREFIX as desired now, set
AUTOBUILD_NANOGUI to ON, and then run cmake configure again.
Call Stack (most recent call first):
src/CMakeLists.txt:80 (UseNanoGUI)
CMakeLists.txt:186 (include)
~~~
This error tells us that cmake could not find the nanogui library pre-installed on our system.  Luckily, MinGfx includes support for downloading, building, and installing nanogui for you using the AUTOBUILD_NANOGUI feature.

Scroll through the list of CMake variables again until you find AUTOBUILD_NANOGUI and click on the checkbox next to it to turn it on.  Now, click Configure again.  This time the configure step will take a while, perhaps 5 minutes, to run because it will be downloading, building, and installing nanogui and all of its dependencies.  

This step should finish without an error.  If you do see an error and you are working with this as part of a UMN course, you may need to stop now and ask for help from a TA or on the class forum -- perhaps there is something we haven't planned for in the particular setup of your computer.


## 6. Generate the Build System / Project Files

Finally, you are ready to press the Generate button.  This is the step that will actually generate the Unix Makefiles, Visual Studio Solution File, or Xcode Project File needed to build MinGfx.  


## 7. Build MinGfx with the Specified Options

Click Open Project if you generated project files for an IDE, or if you generated Unix Makefiles return to your shell and the build directory.  Now, build the project as you normally would in these enviornments.  

For Xcode: `click the Triangle button to build the ALL_BUILD target`

For Visual Studio: In the Solution Explorer Window `right click on ALL_BUILD` and select `Build`

For Unix Makefiles: run `make all`.


## 8. Install the Library to your Directory Tree

For Xcode: `click on ALL_BUILD to reveal a dropdown list and select install` then `click the Triangle button`

For Visual Studio:  In the Solution Explorer Window `right click on INSTALL` and select `Build`.

For Unix Makefiles: run `make install`.

To see if it worked take a look in the lib, include, and bin directories within the root of the tree you setup in Step 1.  You should see the test programs inside bin/ and folders for MinVR-1.0 inside lib/ and include/.


## 9. Run a Test Program

For Xcode: `click on ALL_BUILD to reveal a dropdown list and select mingfx-test-gui-plus-opengl` then `click the Triangle button`

For Visual Studio: In the Solution Explorer Window `right click on mingfx-test-gui-plus-opengl` then selece `Set as StartUp Project`.  Now, `click the green triangle button to run the program`.  IMPORTANT NOTE: When the program runs it will need to find some .dlls that were installed to your `bin` directory in Step 8. So, the bin directory *must be in your windows PATH*.  If you skipped this part of Step 1, go back and add `bin` to your PATH environment variable now.  Then, you may need to restart Visual Studio for it to recognize the new setting.

For Unix Makefiles: from within the build directory run: `tests/mingfx-gui-plus-opengl`




# Create Your Own Graphics Program {#example}

*Prereq:* Before getting started, make sure you have installed MinGfx, going all the way through the 9th step above.

The project in the MinGfx/example folder is an example of a stand-alone project that uses a separate cmake build system.  This means we can copy it and move it out of the MinGfx source tree to serve as a starting point for your own project.  Let's do that now.

~~~
cd /Users/courses/csci4611/sw/dev  (or wherever you put your dev directory)
cp -r MinGfx/example/ mingfx-example
~~~

Now, before we build the example, we need to make sure it knows where to find MinGfx.  
~~~
cd mingfx-example
open up the CMakeLists.txt file in your favorite text editor
~~~

Near the top of the file, find the following lines
~~~
# !!!!!!!!!!!!! EDIT THE FOLLOWING LINE AS NEEDED !!!!!!!!!!!!! 
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../build/install ../../..)
~~~
and add a path to the end of the list to point to the ROOT of your directory tree, for example:
~~~
# !!!!!!!!!!!!! EDIT THE FOLLOWING LINE AS NEEDED !!!!!!!!!!!!! 
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../build/install ../../.. /Users/keefe/courses/cs4611/sw)
~~~

Now, we're ready to build this example using the same cmake idiom as before:
~~~
mkdir build
cd build
cmake-gui ..
~~~
Then, click *Configure*.  If you want to install this example program, you may now edit the CMAKE_INSTALL_PREFIX as before and rerun *Configure*.  It should not be necessary to change any of the other cmake variables for this example program.

Next, click *Generate* to produce project files for your IDE or makefiles.  

Next, build the example program within your IDE or by running `make`.

Finally, you can run your first example graphics program with the command:
~~~
./mingfx-example
~~~

You can change the name of the executable, add more header and source files to the project and make other changes to the build process by editing the CMakeLists.txt file, but remember that after each change, you must run CMake's *Configure* and *Generate* steps again.  

To reiterate the point above, if you are using an IDE, it is important to remember to add new files to your project by editing the CMakeLists.txt file NOT by using File->Add To Project.. or whatever option is provided inside your IDE.  Since we using CMake to generate the IDE's project files, and we may want to switch to a different IDE or to Makefiles in the future, the CMakeLists.txt file needs to be treated as the master record of all of the build settings.

