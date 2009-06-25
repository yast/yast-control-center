
set(kcmyast_SRCS kcmyast.cpp)

include_directories(${KDE4_INCLUDES})

kde4_add_plugin(kcm_yast ${kcmyast_SRCS})
target_link_libraries(kcm_yast ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KNEWSTUFF2_LIBS} ${KDE4_KUTILS_LIBRARY})

install(TARGETS kcm_yast  DESTINATION ${PLUGIN_INSTALL_DIR} )

########### install files ###############
install( FILES kcmyast.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES emoticons.knsrc  DESTINATION  ${CONFIG_INSTALL_DIR} )
