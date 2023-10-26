# dpVision

Currently runs on windows only, compiles using MinGW or MSVC++.

## prerequisites

* Qt5
* zlib if QtZlib is not found
* QuaZip
* boost
  * install boost package if your system can do it... (it is the fastest and most elegant way).
  * or alternatively download current release from <https://www.boost.org/> and unpack it to 3rdParty folder (it is fast too)
  * or alternatively (much slower but most recent sources) enter to 3rdParty folder and clone boost into it:
    * git clone --recurse-submodules <https://github.com/boostorg/boost>
  * if you choosed second or third way, enter the 3rdParty folder and execute two commands (change slash to backslash on Windows):
    * ./bootstrap
    * ./b2
* Eigen3
