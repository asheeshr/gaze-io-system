GIOS - Gaze-IO-System
==============


Goal
----
This software aims to enable desktop navigation and application control using user's eye gaze as an input using only the onboard webcam and in variable lighting conditions. To accomplish this, the software utilizes various algortihms at different stages of the process. We are aiming to develop efficient accurate eye detection and 2D gaze estimation algorithmsfor which we started out with analyzing existing algorithms in the domain and at present, have shifted to developing custom time efficient algorithms. 

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


Progress
--------


Building and Executing
----------------------

Authors
-------
