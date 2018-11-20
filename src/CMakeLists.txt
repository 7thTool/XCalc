#XCalc/CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#CMAKE_C_COMPILER：指定C编译器
#CMAKE_CXX_COMPILER：指定C++编译器
IF(WIN32)
	#需要兼容XP时,定义_WIN32_WINNT 0x0501
	ADD_DEFINITIONS(-D_WIN32_WINNT=0x0601)
	#不用加EXPORTS定义，cmake会自动加DataEngine_EXPORTS
  #ADD_DEFINITIONS(-DDataEngine_EXPORTS)
	#SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
	#SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	#SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
	#SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MT")
	#VS版本的控制可以采用如下变量
	IF(MSVC_VERSION GREATER 1500)
		ADD_COMPILE_OPTIONS(/bigobj)
	ENDIF()
ELSE()
	ADD_DEFINITIONS(-std=c++11)
	ADD_DEFINITIONS(-fpermissive)
	ADD_DEFINITIONS(-fPIC)
	#-w的意思是关闭编译时的警告，也就是编译后不显示任何warning
	#-Wall选项意思是编译后显示所有警告
	#-W选项类似-Wall，会显示警告，但是只显示编译器认为会出现错误的警告
	#调试信息格式主要有下面几种：stabs，COFF，PE-COFF，OMF，IEEE-695和DWARF
	#其中DWARF在Linux中被普遍使用，dwarf2对dwarf1的改变很大，dwarf3大多是对dwarf2的扩充，可以支持C、C++、JAVA、Fortran等语言
	#使用readelf –w* DataEngine命令，*是调试节名的第一个字母，如-wi就是查看.debug_info节的内容，-wl就是查看.debug_line节的内容
	#-g、-ggdb、-g3和-ggdb3，-g产生OS native format的debug信息，GDB可以使用之。而-ggdb产生的debug信息更倾向于给GDB使用的
	#如果你用的GDB调试器，那么使用-ggdb选项。如果是其他调试器，则使用-g。3只是debug信息级别，3这个级别可以调试宏。
	#SET(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 ${CMAKE_CXX_FLAGS_DEBUG} -O0 -w -gdwarf-2 -ggdb3")
	#SET(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 ${CMAKE_CXX_FLAGS_RELEASE} -O2 -Wall -DNODEBUG -gdwarf-2 -ggdb")
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -W -g3")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -W")
	SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O2 -W -gdwarf-3 -g")
	SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os -W")
  INCLUDE_DIRECTORIES(/usr/local/include)
	LINK_DIRECTORIES(/usr/local/lib)
ENDIF()
CMAKE_POLICY(SET CMP0015 NEW)

#使用cmake find_package 查找boost库位置
#FIND_PACKAGE(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})
FIND_PACKAGE(Boost REQUIRED)
IF(Boost_FOUND)
	MESSAGE(STATUS "Boost library status:")
	MESSAGE(STATUS "     version: ${Boost_VERSION}")
	MESSAGE(STATUS "     include path: ${Boost_INCLUDE_DIRS}")
  SET(Boost_LIBRARY_DIRS "$ENV{BOOST_ROOT}/lib")
	MESSAGE(STATUS "     library path: ${Boost_LIBRARY_DIRS}")
	IF(WIN32)
	ELSE()
  	ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
  ENDIF()
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
	LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ELSE()
	MESSAGE(FATAL_ERROR "BOOST library not found")
ENDIF()
#对于系统预定义的Find<NAME>.cmake模块，使用方法一般如上例所示：
#每一个模块都会定义以下几个变量
#<NAME>_FOUND
#<NAME>_INCLUDE_DIRS or <NAME>_INCLUDES
#<NAME>_LIBRARY or <NAME>_LIBRARIES or <NAME>_LIBS
#<NAME>_DEFINITIONS
#你可以通过<NAME>_FOUND来判断模块是否被找到，如果没有找到，按照工程的需要关闭
#某些特性、给出提醒或者中止编译，上面的例子就是报出致命错误并终止构建

SET(lib_SRCS 
    ./XCalc.cpp
)
FOREACH(F ${lib_SRCS})
MESSAGE(${F})
ENDFOREACH(F)


#添加头文件搜索路径
INCLUDE_DIRECTORIES(../include ../../XBoost/helper)
#添加库文件搜索路径
LINK_DIRECTORIES(../lib ${CMAKE_BINARY_DIR}/bin)

IF(WIN32)
	#
ELSE()
	FIND_LIBRARY(BOOST_LOG_SETUP boost_log_setup ${Boost_LIBRARY_DIRS})
	FIND_LIBRARY(BOOST_LOG boost_log ${Boost_LIBRARY_DIRS})
	FIND_LIBRARY(BOOST_REGEX boost_regex ${Boost_LIBRARY_DIRS})
	FIND_LIBRARY(BOOST_DATE_TIME boost_date_time ${Boost_LIBRARY_DIRS})
	FIND_LIBRARY(BOOST_CHRONO boost_chrono ${Boost_LIBRARY_DIRS})
	# 链接动态库
	#SET (EXTRA_LIBS ${EXTRA_LIBS} pthread)
	# 链接静态库
	SET (EXTRA_LIBS ${EXTRA_LIBS} ${BOOST_LOG_SETUP} ${BOOST_LOG} ${BOOST_REGEX} ${BOOST_DATE_TIME} ${BOOST_CHRONO}
	 boost_system boost_filesystem boost_thread pthread)
	SET (EXTRA_LIBS ${EXTRA_LIBS} ssl crypto)
ENDIF()

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

#静态库
ADD_LIBRARY(XCalc STATIC ${lib_SRCS})


