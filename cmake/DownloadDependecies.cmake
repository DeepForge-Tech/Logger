# jsoncpp
FetchContent_Declare(jsoncpp
        GIT_REPOSITORY https://github.com/DeepForge-Technology/jsoncpp.git
        GIT_TAG master
)
# if(NOT jsoncpp_POPULATED)
#   FetchContent_Populate(jsoncpp)
#   add_subdirectory(${jsoncpp_SOURCE_DIR} ${jsoncpp_BINARY_DIR})
# endif()
FetchContent_MakeAvailable(jsoncpp)

FetchContent_Declare(Client
        GIT_REPOSITORY https://github.com/DeepForge-Tech/LogClient.git
        GIT_TAG master
)
FetchContent_MakeAvailable(Client)