GIOS - Gaze-IO-System
==============


Goal
----
This software aims to enable desktop navigation and application control using user's eye gaze as an input using only the onboard webcam and in variable lighting conditions. To accomplish this, the software utilizes various algortihms at different stages of the process. We are aiming to develop efficient accurate eye detection and 2D gaze estimation algorithms for which we started out with analyzing existing algorithms in the domain and at present, have shifted to developing custom time efficient algorithms. 

The 2D gaze estimation software will work as a single application emulating an input device that will interact and control the desktop environment by performing basic manipulations, like navigation, and application manipulation. Initially, the system will be targeted towards Linux based distros. 

Dependencies
------------
The project has the following dependencies for building:
- OpenGL
- OpenMP
- OpenCV compiled using OpenGL, OpenMP, and Gtk. Also, OpenCL is preferable but not necessary.

The project also uses X11 API, and hence will only run on distros that utilise X11 display servers.

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

OpenMP will also be incorporated in the main program logic to increase parallelization of the program code, and increase responsiveness. It will generate more threads based on number of cores available.

Progress
--------
The software is being developed at a brisk pace. We have developed and tested an algorithm for eye detection, and have since moved to developing our own custom algorithm in it's place as we were not satisfied with its performance. It is currently being developed. We have also implemented an initial function for mapping the eye position to an intermediate vector. The final stage of mapping this interemediate vector to a position vector on screen is being developed. Lastly, input driver emulation will be implmented once the previous modules have stabilised.

We have completed the GUI for debugging and mapping the pointer on screen for demonstration purposes.


Building and Executing
----------------------

Git clone or download this repository.

Create a directory under the root directory named `bin`. Then, run `make`. If you encounter an error stating any `.o` file is not found, run `make` again. 

To run the software, from the project root directory, run `./gios`


Authors
-------
Asheesh Ranjan
