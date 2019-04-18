# peakFit
Anaylze TOF spectra averaged by TOF-Tracer (https://github.com/lukasfischer83/TOF-Tracer) to generate masslists.

## Requirements
Needs Qt5, eigen3, libhdf5 and qtcustomplot.
For static compilation (no dlls, running on every system) you need a statically compiled Qt5.


### Compilation on Linux/Ubuntu:
Download and install Qt5 and libhdf5 ( sudo apt install qtcreator libhdf5-serial )
Unpack libs.zip to parent folder of source like this:

peakFit
   main.cpp
   ...
libs
   qcustomplot
   eigen-eigen232....

start QT creator, load project file and run qmake and build all

### Compilation on Windows:
Download and install Qt5 with MinGW as compiler
Download eigen3 and extract next to peakFit directory

#### Building HDF5 with CMake and MinGW packaged with Qt Distribution
(Tested with CMake-hdf5-1.8.16, confirmed not working on >1.10?)
WARNING: Don't use any blank spaces in directory names, otherwhise compilation will fail (HDF5)!
Download CMake HDF5 source and extract (https://www.hdfgroup.org/downloads/hdf5/source-code/ --> Building HDF5 with CMake)
Get CMake and install with default options (Add to PATH)
Add C:\Qt\Tools\mingwXXX_32\bin to PATH (Windows System Properties --> Advanced --> Environment Variables --> System Variables --> Path)
REBOOT to have mingw in Path
##### Generate a makefile for MinGW Build
- Start CMakeGui
- Choose "Browse Source..." and point to HDF5 source folder
- Choose "Browse Build..." and select a target for the binaries (Warning: does not create a subfolder, so create one!)
- specify Generator: MinGW Makefiles
- select "Specify native compilers"
- browse to compilers in C:/Qt/Tools/mingwXXX_32/bin/ (for default Qt installation) ( C:gcc, C++:g++, Fortran:gfortran)
- Uncheck unnecessary configure flags: HDF5_BUILD_EXAMPLES, HDF5_BUILD_TOOLS
- Re-run Configure
- Run Generate
##### Compile:
- in a shell go to target path choosen in CMakeGui
- run mingw32-make.exe
- Grab a coffee

The libs should now be in the target\bin folder.

#### Compile in Qt Creator
- Open Project
- Adjust include Paths for eigen3 and HDF5 Lib in manualPeakFitter.pro
- Run Qmake
- Compile

#### Optional: Compile Qt Statically and create standalone .exe
TODO
