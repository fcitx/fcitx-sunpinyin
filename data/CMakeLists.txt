add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/desc.po
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/getdescpo ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(desc ALL DEPENDS desc.po)

install(FILES fcitx-sunpinyin.conf DESTINATION share/fcitx/data/addon)
install(FILES fcitx-sunpinyin.desc DESTINATION share/fcitx/data/addon)
install(FILES sunpinyin-logo.png DESTINATION share/fcitx/skin/dark RENAME fcitx-sunpinyin.png)
install(FILES sunpinyin-logo-small.png DESTINATION share/fcitx/skin/default RENAME fcitx-sunpinyin.png)
install(FILES sunpinyin-logo-small.png DESTINATION share/fcitx/skin/classic RENAME fcitx-sunpinyin.png)
