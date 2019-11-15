# include( embedded_build.pri ) before this one

qtHaveModule(widgets):!embedded_build {
    CONFIG += have_opengl_debug_window
    QT += widgets
}
