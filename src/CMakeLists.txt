INCLUDE_DIRECTORIES(/usr/include/initng ${CMAKE_CURRENT_BINARY_DIR})

SET(THEMEDIR "/usr/share/YaST2/theme")

ADD_DEFINITIONS(-DQT_NO_DEBUG_OUTPUT)
ADD_DEFINITIONS("-DLOCALEDIR=\"${CMAKE_INSTALL_PREFIX}/share/YaST2/locale\"")
ADD_DEFINITIONS("-DICON_DIR=\"${THEMEDIR}/current/icons/32x32/apps\"")
ADD_DEFINITIONS("-DFALLBACK_ICON_DIR=\"${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/32x32/apps\"")
ADD_DEFINITIONS("-DPIXMAP_DIR=\"${THEMEDIR}/current/icons/32x32/apps\"")
ADD_DEFINITIONS("-DDESKTOP_TRANSLATIONS_DIR=\"${CMAKE_INSTALL_PREFIX}/share/locale\"")
ADD_DEFINITIONS("-DMODULES_DESKTOP_DIR=\"${CMAKE_INSTALL_PREFIX}/share/applications/YaST2\"")
ADD_DEFINITIONS("-DGROUPS_DESKTOP_DIR=\"${CMAKE_INSTALL_PREFIX}/share/applications/YaST2/groups\"")

SET(yast2-control-center_SRCS
    main.cpp
    main_window.cpp
    kcategorizedview.cpp
    kcategorydrawer.cpp
    kcategorizedsortfilterproxymodel.cpp
    moduleiconitem.cpp
    yccmoduleinfoprovider.cpp
    yqdesktopfilesmodel.cpp
    yqmodulegroupsmodel.cpp
    yqmodulesmodel.cpp
    yqmodulesproxymodel.cpp
    yqsavelogs.cpp
    i18n.cpp
)

#SET(yast2-control-center_UIS
#    main_window.ui
#)

SET(yast2-control-center_HDRS
    main_window.h
    kcategorizedview.h
    kcategorydrawer.h
    kcategorizedsortfilterproxymodel.h
    moduleiconitem.h
    kcategorizedview_p.h
    yccmoduleinfoprovider.h
    yqdesktopfilesmodel.h
    yqmodulegroupsmodel.h
    yqmodulesmodel.h
    yqmodulesproxymodel.h
    listview.h
    yqsavelogs.h
    i18n.h
)

#SET(yast2-control-center_RCCS ../files.qrc)


SET(CMAKE_AUTOMOC ON)

ADD_EXECUTABLE(y2controlcenter ${yast2-control-center_SRCS} ${yast2-control-center_MOC} ${yast2-control-center_RCC_SRCS})
TARGET_LINK_LIBRARIES(y2controlcenter Qt5::Widgets Qt5::Gui)
INSTALL(TARGETS y2controlcenter DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/YaST2/bin)

INSTALL(FILES yast.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps )
INSTALL(FILES YaST.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications )
INSTALL(FILES YaST-systemsettings.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/kde4/services )

SET(MODELS_TEST_SRCS
  models_test.cpp
  yqdesktopfilesmodel.cpp
  yqmodulegroupsmodel.cpp
  yccmoduleinfoprovider.cpp
  yqmodulesmodel.cpp )


ADD_EXECUTABLE(models_test ${MODELS_TEST_SRCS})
TARGET_LINK_LIBRARIES(models_test Qt5::Core Qt5::Widgets Qt5::Gui)

FIND_PACKAGE(KDE4)
IF ( KDE4_FOUND )
  ADD_SUBDIRECTORY(kde)
ENDIF ( KDE4_FOUND )
