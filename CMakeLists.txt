cmake_minimum_required(VERSION 3.12)
project(acuracyCalculation)

set(CMAKE_CXX_STANDARD 11)

add_executable(acuracyCalculation main.cpp PointNodeStructure.cpp FileReader.cpp FileReader.h PointNodesComparison.cpp PointNodesComparison.h
        Matrix3D.h Vector3D.h  CTree.h LineDistence.h SpaceColonizationHeader.h
        Matrix3D.cpp  Vector3D.cpp SpaceColonizationHeader.cpp LineDistence.cpp CTree.cpp OCtree.cpp OCtree.h Utils.cpp Utils.h)

#Where to find CMake modules and OpenCV
set(TARGET_INCLUDE
        "C:\\\\Program Files (x86)\\\\CodeBlocks\\\\MinGW\\\\include"
        "C:\\\\Users\\\\hp.he\\\\CLionProjects\\\\Treeanimation\\\\header"
        )

include_directories(acuracyCalculation ${TARGET_INCLUDE})

#add libs you need
set(TARGET_LIBS
        "C:\\\\Program Files (x86)\\\\CodeBlocks\\\\MinGW\\\\lib\\\\libglaux.a"
        "C:\\\\Program Files (x86)\\\\CodeBlocks\\\\MinGW\\\\lib\\\\libopengl32.a"
        "C:\\\\Program Files (x86)\\\\CodeBlocks\\\\MinGW\\\\lib\\\\libglaux.a"
        "C:\\\\Program Files (x86)\\\\CodeBlocks\\\\MinGW\\\\lib\\\\libglu32.a"
        "C:\\\\Program Files (x86)\\\\CodeBlocks\\\\MinGW\\\\lib\\\\libkernel32.a"
        "C:\\\\Program Files (x86)\\\\CodeBlocks\\\\MinGW\\\\lib\\\\glut32.lib"
        "C:\\\\Program Files (x86)\\\\CodeBlocks\\\\MinGW\\\\lib\\\\glew32.lib"
        "C:\\\\Program Files (x86)\\\\CodeBlocks\\\\MinGW\\\\lib\\\\glew32mx.lib"
        "C:\\\\Program Files (x86)\\\\CodeBlocks\\\\MinGW\\\\lib\\\\glew32mxs.lib"
        "C:\\\\Program Files (x86)\\\\CodeBlocks\\\\MinGW\\\\lib\\\\glew32s.lib"
        "C:\\\\Program Files (x86)\\\\CodeBlocks\\\\MinGW\\\\lib\\\\glew32s.lib"
        "C:\\\\Program Files (x86)\\\\CodeBlocks\\\\MinGW\\\\lib\\\\glut.lib"
        "C:\\\\Program Files (x86)\\\\CodeBlocks\\\\MinGW\\\\lib\\\\AntTweakBar.lib"
        )

# linking
target_link_libraries(acuracyCalculation ${TARGET_LIBS} ${OWNDEFINE})