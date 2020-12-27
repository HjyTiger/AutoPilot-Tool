CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

PROJECT(AutoPilot-tool)

set(APP_NAME "AutoPilot-tool")

#set build mode;
#=================================================================
#SET(CMAKE_BUILD_TYPE DEBUG)
SET(CMAKE_BUILD_TYPE RELEASE)
#=================================================================

#Qt5 settings
#=================================================================
FIND_PACKAGE(Qt5  COMPONENTS Widgets Gui Xml OpenGL PrintSupport REQUIRED)
FIND_PACKAGE(Qt5Widgets)
FIND_PACKAGE(Qt5Gui)
INCLUDE_DIRECTORIES(/usr/include/x86_64-linux-gnu/qt5/QtCore)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC             ON)
ADD_DEFINITIONS(-DQCUSTOMPLOT_USE_OPENGL)
#=================================================================

#Boost
#=================================================================
#using system time and pthread instead of boost, remove boost-lib using in 20191220
FIND_PACKAGE(Boost 1.54 COMPONENTS thread system filesystem timer chrono REQUIRED)
#=================================================================

#OpenCV
#=================================================================
#FIND_PACKAGE(OpenCV REQUIRED)
#=================================================================

#cmake property settings
#=================================================================
set(ui_path "${CMAKE_CURRENT_SOURCE_DIR}/build/ui_mainwindow.h")
set_property(SOURCE ${ui_path} PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE ${ui_path} PROPERTY SKIP_AUTOUIC ON)
set_property(SOURCE ${ui_path} PROPERTY SKIP_AUTOGEN ON)

#set_property(SOURCE file.h PROPERTY SKIP_AUTOMOC ON)

#=================================================================

#c++11
#=================================================================
add_definitions(-std=c++11)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
#=================================================================

#cmake settings
#=================================================================

MESSAGE(STATUS "current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")

#set include files;
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/
                    ${CMAKE_CURRENT_SOURCE_DIR}/include/
                    ${CMAKE_CURRENT_SOURCE_DIR}/view
                    ${CMAKE_CURRENT_SOURCE_DIR}/view/QtPropertyBrowser
                    ${CMAKE_CURRENT_SOURCE_DIR}/config
                    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/x86/include/
                    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/x86/include/opencv4
                    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/x86/include/glib-2.0/
                   )

#set include files: *.h, *.hpp;
FILE(GLOB_RECURSE PROJECT_HEADERS "include/*.h" 
                                  "include/*.hpp"
                                  "config/*.h")

#set source files:*.cpp;
FILE(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp" 
                                  "src/*.c" 
                                  "src/*.cc" 
                                  "config/*.cpp" 
                                  "config/*.cc"
                                  "Model/*.cpp" 
                                  "Model/*.cc"
                                  "view/*.cc"
                                  "view/*.cpp"
                                  "Communicate/message/protobuf/sample/*.cc")
#AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/src/ PROJECT_SOURCES)

#set library files: *.lib
FILE(GLOB_RECURSE PROJECT_LIBRARIES "lib/*.a" "lib/*.so")

#set library files path;   
LINK_DIRECTORIES(/usr/lib/x86_64-linux-gnu)
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/x86/lib/)
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib/)
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib/Release)

#set library output directory;
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
#=================================================================


#set qt wrap files;
#=================================================================
#qt moc headers;
QT5_WRAP_CPP(PROJECT_MOC_HEADERS ${PROJECT_HEADERS})

#qt moc ui;
FILE(GLOB_RECURSE PROJECT_UIS "${PROJECT_SOURCE_DIR}/ui/*.ui")
QT5_WRAP_UI(PROJECT_MOC_UIS ${PROJECT_UIS})

#qt moc qrc;
FILE(GLOB_RECURSE PROJECT_QRCS "ui/*.qrc")
QT5_ADD_RESOURCES(PROJECT_MOC_QRCS ${PROJECT_QRCS})
#=================================================================

SET(PROJECT_LIBRARIES)
SET(OpenCV_LIBS   opencv_core
	              opencv_highgui
	              opencv_imgcodecs
	              opencv_imgproc)

#build release executable and library
if(${CMAKE_BUILD_TYPE} MATCHES "RELEASE")
    #build executable
    ADD_EXECUTABLE(${APP_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${PROJECT_MOC_UIS} ${PROJECT_MOC_UIS_HEADERS} ${PROJECT_MOC_QRCS})
    TARGET_LINK_LIBRARIES(${APP_NAME} 
                          ${OpenCV_LIBS}
                          ${PROJECT_LIBRARIES} 
                          Qt5::Widgets 
                          Qt5::Gui 
                          Qt5::Core 
                          Qt5::PrintSupport
                          lcm 
                          protobuf-lite 
                          protobuf 
                          ${PROJECT_LIBRARIES} 
                          ${Boost_LIBRARIES} 
                          gthread-2.0 
                          glib-2.0 
                          GL 
                          GLU
                          assimp 
                          3ds
                          jsoncpp)

    SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/Release)

    #install built executable and library;
    INSTALL(TARGETS ${APP_NAME}
            RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/Release #executable;
            ARCHIVE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib #dynamic lib, *.a;
            LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib #static lib, *.so;
    )
#build debug executable and library
else()
    #build executable
    ADD_EXECUTABLE(${APP_NAME}_d ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${PROJECT_MOC_UIS} ${PROJECT_MOC_UIS_HEADERS} ${PROJECT_MOC_QRCS})
    TARGET_LINK_LIBRARIES(${APP_NAME}_d 
                          ${OpenCV_LIBS}
                          ${PROJECT_LIBRARIES} 
                          Qt5::Widgets 
                          Qt5::Gui 
                          Qt5::Core 
                          Qt5::PrintSupport
                          lcm 
                          protobuf-lite 
                          protobuf 
                          ${PROJECT_LIBRARIES} 
                          ${Boost_LIBRARIES} 
                          gthread-2.0 
                          glib-2.0 
                          GL 
                          GLU
                          assimp 
                          3ds
                          jsoncpp)

    SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/Debug)

    #install built executable and library;
    INSTALL(TARGETS ${APP_NAME}_d#
            RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug #executable;
            ARCHIVE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib #dynamic lib, *.a;
            LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib #static lib, *.so;
    )
endif()
