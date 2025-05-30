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
install(FILES melodeon.metainfo.xml DESTINATION ${CMAKE_INSTALL_PREFIX}/share/metainfo)
install(FILES ../images/melodeon.svg DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps)
install(FILES ../images/melodeon-symbolic.svg DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/symbolic/apps)
update_iconcache(hicolor)
