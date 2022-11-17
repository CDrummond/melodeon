macro (update_iconcache ICON_THEME)
    # Update mtime of hicolor icon theme dir.
    # We don't always have touch command (e.g. on Windows), so instead create
    #  and delete a temporary file in the theme dir.
    install(CODE "
    set(DESTDIR_VALUE \"\$ENV{DESTDIR}\")
    if (NOT DESTDIR_VALUE)
        file(WRITE \"${CMAKE_INSTALL_PREFIX}/share/icons/${ICON_THEME}/temp.txt\" \"update\")
        file(REMOVE \"${CMAKE_INSTALL_PREFIX}/share/icons/${ICON_THEME}/temp.txt\")
    endif (NOT DESTDIR_VALUE)
    ")
endmacro (update_iconcache)
    
install(FILES melodeon.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
install(FILES ../images/melodeon.svg DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps)
update_iconcache(hicolor)
