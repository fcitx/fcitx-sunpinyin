add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/desc.po
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/getdescpo ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(desc ALL DEPENDS desc.po)

install(FILES fcitx-sunpinyin.conf DESTINATION share/fcitx/data/addon)
install(FILES fcitx-sunpinyin.desc DESTINATION share/fcitx/data/addon)
