INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(/usr/include/initng ${CMAKE_CURRENT_BINARY_DIR})

SET(THEMEDIR "/usr/share/YaST2/theme")

ADD_DEFINITIONS("-DLOCALEDIR=\"${CMAKE_INSTALL_PREFIX}/share/YaST2/locale\"")
ADD_DEFINITIONS("-DICON_DIR=\"${THEMEDIR}/current/icons/32x32/apps\"")
ADD_DEFINITIONS("-DPIXMAP_DIR=\"${THEMEDIR}/current/icons/32x32/apps\"")
ADD_DEFINITIONS("-DDESKTOP_TRANSLATIONS_DIR=\"${CMAKE_INSTALL_PREFIX}/share/locale\"")
ADD_DEFINITIONS("-DMODULES_DESKTOP_DIR=\"${CMAKE_INSTALL_PREFIX}/share/applications/YaST2\"")
ADD_DEFINITIONS("-DGROUPS_DESKTOP_DIR=\"${CMAKE_INSTALL_PREFIX}/share/applications/YaST2/groups\"")

SET(yast2-control-center_SRCS
    main.cpp
    main_window.cpp
    kcategorizedview.cpp
 #   menuitem.cpp
    kcategorydrawer.cpp
    kcategorizedsortfilterproxymodel.cpp
  #  kcmodulemodel.cpp
    moduleiconitem.cpp
  #  kcmoduleinfo.cpp

    yccmoduleinfoprovider.cpp

    yqdesktopfilesmodel.cpp
    yqmodulegroupsmodel.cpp
    yqmodulesmodel.cpp
    yqmodulesproxymodel.cpp

    i18n.cpp
)

#SET(yast2-control-center_UIS
#    main_window.ui
#)

SET(yast2-control-center_HDRS
    main_window.h
    kcategorizedview.h
  #  menuitem.h
  #  kcmodulemodel.h
    kcategorydrawer.h
    kcategorizedsortfilterproxymodel.h
    moduleiconitem.h
  #  kcmoduleinfo.h
    kcategorizedview_p.h
  
    yccmoduleinfoprovider.h

    yqdesktopfilesmodel.h
    yqmodulegroupsmodel.h
    yqmodulesmodel.h
    yqmodulesproxymodel.h

    i18n.h
)

#SET(yast2-control-center_RCCS ../files.qrc)


QT4_AUTOMOC(${yast2-control-center_SRCS})
# QT4_WRAP_UI(yast2-control-center_UIS_H ${yast2-control-center_UIS})
QT4_WRAP_CPP(yast2-control-center_MOC ${yast2-control-center_UIS_H})
QT4_ADD_RESOURCES(yast2-control-center_RCC_SRCS ${yast2-control-center_RCCS})

#ADD_DEFINITIONS(-DQT_NO_DEBUG)


ADD_EXECUTABLE(y2controlcenter ${yast2-control-center_SRCS} ${yast2-control-center_MOC} ${yast2-control-center_RCC_SRCS})
TARGET_LINK_LIBRARIES(y2controlcenter ${QT_LIBRARIES})
INSTALL(TARGETS y2controlcenter DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/YaST2/bin)

INSTALL(FILES yast.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps )
INSTALL(FILES YaST.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications )

SET(MODELS_TEST_SRCS
  models_test.cpp
  yqdesktopfilesmodel.cpp
  yqmodulegroupsmodel.cpp
  yccmoduleinfoprovider.cpp
  yqmodulesmodel.cpp )


QT4_AUTOMOC(${MODELS_TEST_SRCS})
ADD_EXECUTABLE(models_test ${MODELS_TEST_SRCS})
TARGET_LINK_LIBRARIES(models_test ${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES})

FIND_PACKAGE(KDE4)
IF ( KDE4_FOUND )
  ADD_SUBDIRECTORY(kde)
ENDIF ( KDE4_FOUND )
