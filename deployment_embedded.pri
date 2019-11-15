linux-tegra3-g++|linux-rasp-pi-g++ {
    isEmpty(target.path) {
        target.path = /opt/$${TARGET}/bin
        export(target.path)
    }

    INSTALLS += target
}
