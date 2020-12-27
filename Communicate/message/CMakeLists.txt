CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
PROJECT(message)
#set include file source file lib file in this project
FILE(GLOB_RECURSE PROJECT_SOURCES      "protobuf/*.cc")
ADD_LIBRARY(${PROJECT_NAME} STATIC     ${PROJECT_SOURCES} 
                                       ${PROJECT_HEADERS})
