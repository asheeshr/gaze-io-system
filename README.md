GIOS - Gaze-IO-System
==============


Goal
----
This software aims to enable desktop navigation and application control using user's eye gaze as an input using only the onboard webcam and in variable lighting conditions. To accomplish this, the software utilizes various algorithms at different stages of the process. We are aiming to develop efficient accurate eye detection and 2D gaze estimation algorithms. Initially, we started out with analyzing existing algorithms in the domain and presently, have shifted to developing custom time efficient algorithms. 

The 2D gaze estimation software will work as a single application emulating an input device that will interact with and control the desktop environment by performing basic tasks, like navigation, and application manipulation. Initially, the system will be targeted towards Linux based distros. 

Dependencies
------------
The project has the following dependencies for building:
- OpenGL
- OpenMP (implemented by GCC 4.7 and higher)
- OpenCV compiled using OpenGL, OpenMP, and Gtk (requires `libgtkglext`). Also, OpenCL is preferable but not necessary.
- GNUplot

The project also uses X11 API, and hence will only run on distros that utilise X11 display servers.

A [script](https://github.com/asheeshr/gaze-io-system/blob/master/scripts/install-opencv.sh) that configures and installs OpenCV is included in the repo. 


Structure
---------
The software is divided into the following modules:
- `facedetect` contains wrapper functions for calls to OpenCV's Haar cascade filter
- `featuredetect` implements an algorithm for high accuracy eye detection
- `gazeestimate` implmenents the logic for mapping eye gaze movements to an absolute co-ordinate
- `emulatedriver` contains the input driver emulation code that uses udev and kernel API

The other modules contain supporting code:
- `gui` contains a GUI implementation for debugging
- `guipointer` implements a debugging GUI that maps the pointer to the X11 desktop
- `support` (self-explanatory)

The program runs in three main threads:

1. Main Program Logic - processes the images and generates co-ordinates using OpenCV
2. Debugging GUI - uses HighGUI from OpenCV
3. Pointer GUI - using OpenGL 

OpenMP will also be incorporated in the main program logic to increase parallelization of the program code and responsiveness. It will generate more threads based on number of cores available.

Progress
--------
The software is being developed at a brisk pace. We have developed and tested an algorithm for eye detection, and have since moved to developing our own custom algorithm in it's place as we were not satisfied with its performance. It is currently being developed. We have also implemented an initial function for mapping the eye position to an intermediate vector. The final stage of mapping this interemediate vector to a position vector on screen is being developed. Lastly, input driver emulation will be implmented once the previous modules have stabilised.

We have completed the GUI for debugging and mapping the pointer on screen for demonstration purposes.

<img src="https://raw.githubusercontent.com/asheeshr/gaze-io-system/master/data/gios-debug-gui.png">

Building and Executing
----------------------

Git clone or download this repository.

Create a directory under the root directory named `bin`. Then, run `make`. If you encounter an error stating any `.o` file is not found, run `make` again. 

To run the software, from the project root directory, run `./gios`


Authors
-------
Asheesh Ranjan, Pranav Jetley
