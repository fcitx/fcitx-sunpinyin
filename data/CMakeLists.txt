add_custom_command(OUTPUT desc.po
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/getdescpo ${fcitx-sunpinyin_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS fcitx-sunpinyin.desc)

add_custom_target(desc ALL DEPENDS desc.po)

fcitx_add_addon_conf_file(fcitx-sunpinyin.conf)
fcitx_add_configdesc_file(fcitx-sunpinyin.desc)
install(FILES sunpinyin-logo.png DESTINATION share/fcitx/skin/dark RENAME sunpinyin.png)
install(FILES sunpinyin-logo-small.png DESTINATION share/fcitx/skin/default RENAME sunpinyin.png)
install(FILES sunpinyin-logo-small.png DESTINATION share/fcitx/skin/classic RENAME sunpinyin.png)
