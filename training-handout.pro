TEMPLATE=subdirs

!ios:include(configtests.pri)

SUBDIRS= \
#   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
   \
#  qtmobility \
   \
   \
   \
   \
  debugging \
   \
  profiling \
   \
   \
   \
   \
  

qtHaveModule(qtxmlpatterns) {
  
}

greaterThan(QT_MAJOR_VERSION, 4) {
  greaterThan(QT_MINOR_VERSION, 7) {
    qtHaveModule(waylandcompositor) {
      SUBDIRS += 
    }
  }
}

qtHaveModule(script) {
  SUBDIRS += 
} else {
  message ( "QtScript not available")
}

greaterThan(QT_MAJOR_VERSION, 4) {
  greaterThan(QT_MINOR_VERSION, 4) {
    qtHaveModule(webengine) {
      message( "Adding QtWebEngine examples" )
      SUBDIRS += 
    } else {
      message( "Skipping QtWebEngine examples (requires  module)" )
    }
  } else {
    message( "Skipping QtWebEngine examples (require Qt >= 5.5)" )
  }
} else {
  message( "Skipping QtWebEngine examples (require Qt >= 5.5)" )
}

contains(CONFIG, config_phonon) {
  message( "Adding  examples" )
  SUBDIRS += 
} else {
  message( "Skipping  examples (require Qt built with Phonon support)" )
}

contains(CONFIG, config_desktopgl) {
  greaterThan(QT_MAJOR_VERSION, 4) {
    greaterThan(QT_MINOR_VERSION, 3) {
      message( "Adding Desktop OpenGL examples" )
      SUBDIRS += 
    } else {
      message( "Skipping Desktop OpenGL examples (require Qt >= 5.4)" )
    }
  } else {
    message( "Skipping Desktop OpenGL examples (require Qt >= 5.4)" )
  }
} else {
  message( "Skipping Desktop OpenGL examples (require Qt built with Desktop OpenGL support)" )
}

greaterThan(QT_MAJOR_VERSION, 4) {
  message( "Compiling against Qt5" )
  greaterThan(QT_MINOR_VERSION, 4) {
    message( "Adding OpenGL examples" )
    SUBDIRS += opengl
  } else {
    message( "Skipping OpenGL examples (require Qt >= 5.5)" )
  }
} else {
  message( "Skipping OpenGL examples (require Qt >= 5.5)" )
}

packagesExist(QtAndroidExtras) {
   message("Adding  examples")
   SUBDIRS += 
} else {
   message( "Skipping  examples (requires module QtAndroidExtras)" )
}

ios: SUBDIRS += 

win32-msvc2013 {
  message( "Skipping Qt 3D examples (require MSVC >= 2015 and Qt >= 5.10)" )
} else {
  greaterThan(QT_MAJOR_VERSION, 4) {
    greaterThan(QT_MINOR_VERSION, 9) {
        exists($$PWD/opengl/assets/extra/qt3d/assets.pri) {
            contains(CONFIG, config_qt3d) {
              message( "Adding Qt 3D examples" )
              SUBDIRS += 
            } else {
              message( "Skipping Qt 3D examples (requires Qt3DQuickExtras/Qt3DAnimation headers -- note: e.g. Ubuntu 18.10 does not ship them due to https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=895386)" )
            }
        } else {
            message( "Skipping Qt 3D examples (require addon/opengl/assets/extra submodule). Run: 'git submodule update --init --recursive' to clone it.")
        }
    } else {
      message( "Skipping Qt 3D examples (require Qt >= 5.10)" )
    }
  } else {
    message( "Skipping Qt 3D examples (require Qt >= 5.10)" )
  }
}

greaterThan(QT_MAJOR_VERSION, 4) {
  greaterThan(QT_MINOR_VERSION, 7) {
    qtHaveModule(serialbus) {
      SUBDIRS += 
    } else {
      message( "Skipping Qt Automotive examples (serialbus module not found)" )
    }
  } else {
    message( "Skipping Qt Automotive examples (require Qt >= 5.8)" )
  }
} else {
  message( "Skipping Qt Automotive examples (require Qt >= 5.8)" )
}

greaterThan(QT_MAJOR_VERSION, 4) {
  greaterThan(QT_MINOR_VERSION, 7) {
    qtHaveModule(sxcml) {
      SUBDIRS += 
    } else {
      message( "Skipping Qt SCXML examples (module not found)" )
    }
  } else {
    message( "Skipping Qt SCXML examples (require Qt >= 5.8)" )
  }
} else {
  message( "Skipping Qt SCXML examples (require Qt >= 5.8)" )
}

greaterThan(QT_MAJOR_VERSION, 4) {
  greaterThan(QT_MINOR_VERSION, 8) {
    SUBDIRS += 
  } else {
    message( "Skipping QtQuick Controls 2 examples (require Qt >= 5.9)" )
  }
} else {
  message( "Skipping QtQuick Controls 2 examples (require Qt >= 5.9)" )
}

