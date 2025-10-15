cmake_minimum_required(VERSION 3.26)
project(A2_345)

set(CMAKE_CXX_STANDARD 20)

# 👇 tell CLion where your Assignment 1 headers live
include_directories(
        .
        /Users/arshdeepbhullar/Downloads/Warzone_Submission-3
)

# 👇 compile your A2 files + GameEngine.cpp from A1
add_executable(A2_345


        /Users/arshdeepbhullar/Downloads/Warzone_Submission-3/GameEngine.cpp
        /Users/arshdeepbhullar/Downloads/Warzone_Submission-3/Player.cpp
        /Users/arshdeepbhullar/Downloads/Warzone_Submission-3/Orders.cpp
        /Users/arshdeepbhullar/Downloads/Warzone_Submission-3/Map.cpp
        /Users/arshdeepbhullar/Downloads/Warzone_Submission-3/Cards.cpp
        CommandProcessing.h
        CommandProcessing.cpp
        CommandProcessingDriver.cpp

)

