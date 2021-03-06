##Gaze I/O System - OpenCV Installer
#
#Copyright (C) 2014 Asheesh Ranjan, Pranav Jetley, Osank Jain,
#Vasu Bhardwaj, Varun Kalra
#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.
# 
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License along
#with this program; if not, write to the Free Software Foundation, Inc.,
#51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
echo "This script will download, build and install OpenCV 2.4.10 on your system"
echo "This script may take upto an hour depending on your internet speed and processor capability"
echo "Manual intervention will be required in between the process."

if [ ! -d ~/opencv ]; then
    mkdir ~/opencv
fi

cd ~/opencv
touch ./installer-log.txt #Log file

echo "(1/5) Checking for OpenCV zip file"
if [ -f ./"OpenCV-2.4.10.zip" ]; then
    echo "Found OpenCV zip file"
else
    echo "Downloading OpenCV 2.4.10"
    wget -O OpenCV-2.4.10.zip http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/2.4.10/opencv-2.4.10.zip/download > ./installer-log.txt
fi

#echo "Installing Dependencies"
#Add install command for all dependencies
sudo apt-get install build-essential
sudo apt-get install cmake checkinstall libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
#sudo apt-get install libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev libv4l-dev v4l-utils 
sudo apt-get install libgtkglext1 libgtkglext1-dev #for libgtk
sudo apt-get install freeglut3-dev #for opengl
sudo apt-get install libgomp1 #for opemmp

echo "(2/5) Extracting files from OpenCV zip file"
unzip ./OpenCV-2.4.10.zip
cd ./opencv-2.4.10
mkdir build
cd build

echo "(3/5) Configuring options for OpenCV build"
cmake \
    -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D WITH_TBB=OFF \
    -D BUILD_NEW_PYTHON_SUPPORT=ON \
    -D WITH_V4L=ON \
    -D INSTALL_C_EXAMPLES=OFF \
    -D INSTALL_PYTHON_EXAMPLES=OFF \
    -D BUILD_EXAMPLES=OFF \
    -D WITH_QT=OFF \
    -D WITH_OPENGL=ON \
    -D WITH_OPENMP=ON \
    -D WITH_OPENCL=ON \
    .. >> ../../installer-log.txt
#Need to add arguments for Gtk support

echo "(4/5) Building OpenCV - This will take some time."
make -j$((`nproc`*2)) >> ../../installer-log.txt


echo "(5/5) Build complete. Enter password to install."
sudo make install >> ../../installer-log.txt
sudo ldconfig

echo "OpenCV 2.4.10 should be ready for use. Check log file in case of any errors."
