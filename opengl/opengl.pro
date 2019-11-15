cache()

TEMPLATE = subdirs
SUBDIRS += \
           common \
            \
            \
            \
            \
            \
            \
            \
            \
           


!contains(QT_CONFIG, opengles2) {
    SUBDIRS += \
                \
               performance \
                \
               

    qtHaveModule(quick):SUBDIRS += 
}

for(subdir, SUBDIRS) {
    !equals(subdir, common) {
        $${subdir}.depends += common
    }
}

