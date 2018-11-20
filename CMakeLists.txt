#XCalc/CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(XCalc)

#在CMake中集成vcpkg
#cmake .. -DCMAKE_TOOLCHAIN_FILE=.../vcpkg.cmake
#使用静态库
#1、打开vcxproj工程文件，在xml的段里面增加如下两句话即可
#<VcpkgTriplet>x86-windows-static</VcpkgTriplet>
#<VcpkgEnabled>true</VcpkgEnabled>
#2、添加lib依赖
#$(VcpkgRoot)debug\lib\*.lib
#3、在CMake中集成静态库，需要额外指令
#cmake .. -DCMAKE_TOOLCHAIN_FILE=.../vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x86-windows-static
#cmake -G "Visual Studio 14" ..
#eg:cmake .. -DCMAKE_TOOLCHAIN_FILE=D:\share\vcpkg\scripts\buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -G "Visual Studio 14 Win64"

#版本号，注意0会导致CONFIGURE_FILE替换@VAR@失败
SET(SOURCE_VERSION_MAJOR 1)
SET(SOURCE_VERSION_MINOR 1)
SET(SOURCE_VERSION_BUILD 1)
SET(SOURCE_VERSION "${SOURCE_VERSION_MAJOR}.${SOURCE_VERSION_MINOR}.${SOURCE_VERSION_BUILD}")

CONFIGURE_FILE(./include/version.h.in ${PROJECT_SOURCE_DIR}/include/version.h @ONLY)

ADD_SUBDIRECTORY(../XBoost XBoost)
ADD_SUBDIRECTORY(XCalc)

#ELSE(WIN32)之类的语句很容易引起歧义，可以这样消除这个问题
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)
#然后可以这样使用
IF(WIN32)
	MESSAGE(STATUS "PATH: $ENV{PATH}")
ELSE()
	#使用$ENV{NAME}指令就可以调用系统的环境变量了
	MESSAGE(STATUS "HOME: $ENV{HOME}")
ENDIF()
