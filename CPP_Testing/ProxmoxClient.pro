# Project file (ProxmoxClient.pro)

QT += widgets network

SOURCES += \
    main.cpp \
    ProxmoxApiManager.cpp \
    ProxmoxClientWindow.cpp \
    VmModel.cpp # Removed proxmox_listvms.cpp

HEADERS += \
    ProxmoxApiManager.h \
    ProxmoxClientWindow.h \
    VmModel.h \
    json.hpp

# Add the libcurl linker flag here:
LIBS += -lcurl