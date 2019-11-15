# network needed for asset downloader, etc.
# very likely already brought in by QQ2
QT += openglextensions network

# Enable C++11 support
# Required for gli library for loading DDS/KTX textures
CONFIG += c++11

INCLUDEPATH += \
    $$PWD/3rdparty/gli-0.7.0/gli \
    $$PWD/3rdparty/gli-0.7.0/external/glm-0.9.7.1
