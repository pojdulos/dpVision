# dpVision (data processing for vision)

The dpVision software framework offers a flexible platform for implementing and testing various data and image processing algorithms. It handles multimodal data across various formats and dimensions, acquired with different imaging techniques like 3D scanning or medical imaging. Support for various object types within a common space enables
efficient multimodal image processing and algorithm testing, addressing the limitations of existing solutions.

<img src="./docs/abel01.png" alt="screenshot" width="50%"/>

* The application currently compiles and runs only on Windows
* Easy CMake-based configuration
* Building with MinGW and MSVC++ was tested.

## prerequisites

* Qt5 (https://www.qt.io/), and:
  * QtZlib or zlib if QtZlib is not found
  * QuaZip: https://quazip.sourceforge.net/
* Eigen3 (https://eigen.tuxfamily.org/index.php?title=Main_Page)
* boost
  * install boost package if your system can do it... (it is the fastest and most elegant way).
  * or alternatively download current release from <https://www.boost.org/> and unpack it to 3rdParty folder (it is fast too)
  * or alternatively (much slower but most recent sources) enter to 3rdParty folder and clone boost into it:
    * git clone --recurse-submodules <https://github.com/boostorg/boost>
  * if you choosed second or third way, enter the 3rdParty folder and execute two commands (change slash to backslash on Windows):
    * ./bootstrap
    * ./b2

## Build with MinGW

* clone this repository

``git clone https://github.com/pojdulos/dpVision.git``

* go into project directory

``cd dpVision``

* create build folder and go into it

``mkdir build``

``cd build``

* configure project for MinGW compiler (recommended) with CMake

``cmake  -G"Unix Makefiles" -D CMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release ../src``

* build project

``make``

* run dpVision application

``./dpVision.exe``

* optionaly create portable folder

``make bin``



