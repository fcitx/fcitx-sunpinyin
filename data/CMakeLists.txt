foreach(size 16 22 24 32 48 128)
  install(FILES sunpinyin-logo-${size}.png
    RENAME fcitx-sunpinyin.png
    DESTINATION share/icons/hicolor/${size}x${size}/apps)
endforeach()

install(FILES sunpinyin-logo-16.png
  DESTINATION share/fcitx/skin/default RENAME sunpinyin.png)
install(FILES sunpinyin-logo-16.png
  DESTINATION share/fcitx/skin/classic RENAME sunpinyin.png)
install(FILES sunpinyin-logo-22.png
  DESTINATION share/fcitx/skin/dark RENAME sunpinyin.png)
install(FILES sunpinyin-logo-48.png
  DESTINATION share/fcitx/imicon RENAME sunpinyin.png)
