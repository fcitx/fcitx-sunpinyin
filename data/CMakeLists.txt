add_custom_command(OUTPUT desc.po
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/getdescpo ${fcitx-sunpinyin_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(desc ALL DEPENDS desc.po)

install(FILES fcitx-sunpinyin.conf DESTINATION share/fcitx/data/addon)
install(FILES fcitx-sunpinyin.desc DESTINATION share/fcitx/data/addon)
install(FILES sunpinyin-logo.png DESTINATION share/fcitx/skin/dark RENAME sunpinyin.png)
install(FILES sunpinyin-logo-small.png DESTINATION share/fcitx/skin/default RENAME sunpinyin.png)
install(FILES sunpinyin-logo-small.png DESTINATION share/fcitx/skin/classic RENAME sunpinyin.png)
