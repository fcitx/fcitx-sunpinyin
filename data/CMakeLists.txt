fcitx_add_addon_conf_file(fcitx-sunpinyin.conf)
fcitx_add_configdesc_file(fcitx-sunpinyin.desc)

fcitx_add_inputmethod_conf_file(sunpinyin.conf)

foreach(size 16 22 24 32 48 128)
    install(FILES sunpinyin-logo-${size}.png RENAME fcitx-sunpinyin.png DESTINATION share/icons/hicolor/${size}x${size}/apps)
endforeach(size 16 22 24 32 48 128)
install(FILES sunpinyin-logo-22.png DESTINATION share/fcitx/skin/dark RENAME sunpinyin.png)
install(FILES sunpinyin-logo-16.png DESTINATION share/fcitx/skin/default RENAME sunpinyin.png)
install(FILES sunpinyin-logo-16.png DESTINATION share/fcitx/skin/classic RENAME sunpinyin.png)
