if(WIN32)
  file(GLOB OPENBR_DLLS "${OPENBR_DIR}/bin/*.dll")
  list(APPEND DEPENDENCY_DLLS ${OPENBR_DLLS})
  
  file(GLOB QT_DLLS "${Qt5_DIR}/../../../bin/ic*.dll")
  list(APPEND QT_DLLS "${Qt5_DIR}/../../../bin/Qt5Core.dll")
  list(APPEND QT_DLLS "${Qt5_DIR}/../../../bin/Qt5Widgets.dll")
  list(APPEND QT_DLLS "${Qt5_DIR}/../../../bin/Qt5Gui.dll")
  list(APPEND QT_DLLS "${Qt5_DIR}/../../../bin/Qt5Network.dll")
  list(APPEND QT_DLLS "${Qt5_DIR}/../../../bin/Qt5Sql.dll")
  list(APPEND QT_DLLS "${Qt5_DIR}/../../../bin/Qt5Widgets.dll")
  list(APPEND QT_DLLS "${Qt5_DIR}/../../../bin/Qt5Xml.dll")
  list(APPEND DEPENDENCY_DLLS ${QT_DLLS})
  
  foreach(OPENCV_DLL ${OpenCV_LIBS})
    file(TO_CMAKE_PATH ${_OpenCV_LIB_PATH} OPENCV_DLLS_PATH)
    file(GLOB OPENCV_DLLS "${OPENCV_DLLS_PATH}/${OPENCV_DLL}*")
    list(APPEND DEPENDENCY_DLLS ${OPENCV_DLLS})
  endforeach()
  
  
endif()