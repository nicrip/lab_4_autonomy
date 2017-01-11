#--------------------------------------------------------
# The CMakeLists.txt for:                   lib_behaviors
# Author(s):                                 Nick Rypkema
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
find_package(MOOS 10 REQUIRED)
INCLUDE_DIRECTORIES(${MOOS_INCLUDE_DIRS})
set(MOOS_LIBRARIES MOOS)
find_package(IvP REQUIRED)
include_directories(${IVP_INCLUDE_DIRS})

ADD_LIBRARY(BHV_SandSharkSpeedTest_hdgspd SHARED
   BHV_SandSharkSpeedTest_hdgspd.cpp)

TARGET_LINK_LIBRARIES(BHV_SandSharkSpeedTest_hdgspd
   ${MOOS_LIBRARIES}
   helmivp
   behaviors
   ivpbuild
   logic
   ivpcore
   bhvutil
   mbutil
   geometry
   ${SYSTEM_LIBS} )

ADD_LIBRARY(BHV_SandSharkSpeedTest_dpth SHARED
   BHV_SandSharkSpeedTest_dpth.cpp)

TARGET_LINK_LIBRARIES(BHV_SandSharkSpeedTest_dpth
   ${MOOS_LIBRARIES}
   helmivp
   behaviors
   ivpbuild
   logic
   ivpcore
   bhvutil
   mbutil
   geometry
   ${SYSTEM_LIBS} )

ADD_LIBRARY(BHV_SandSharkTrackTrail_hdgspd SHARED
   BHV_SandSharkTrackTrail_hdgspd.cpp)

TARGET_LINK_LIBRARIES(BHV_SandSharkTrackTrail_hdgspd
   ${MOOS_LIBRARIES}
   helmivp
   behaviors
   ivpbuild
   logic
   ivpcore
   bhvutil
   mbutil
   geometry
   ${SYSTEM_LIBS} )

