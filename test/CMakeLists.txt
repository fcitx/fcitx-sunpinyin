include_directories(${SUNPINYIN_INCLUDE_DIRS})
link_directories(${SUNPINYIN_LIBRARY_DIRS})

add_executable(shuangpin_decode shuangpin_decode.cpp)
target_link_libraries(shuangpin_decode ${SUNPINYIN_LIBRARIES})

add_test(shuangpin_decode shuangpin_decode)
