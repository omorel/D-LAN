# -------------------------------------------------
# Project created by QtCreator 2009-10-05T18:53:58
# -------------------------------------------------
QT += network
QT -= gui
TARGET = UploadManager
TEMPLATE = lib

include(../../Common/common.pri)
include(../../Libs/protobuf.pri)

CONFIG += staticlib \
    link_prl \
    create_prl

INCLUDEPATH += . \
    ../..

DEFINES += UPLOADMANAGER_LIBRARY
SOURCES += priv/UploadManager.cpp \
    Builder.cpp \
    priv/Log.cpp \
    priv/Upload.cpp
HEADERS += IUploadManager.h \
    IUpload.h \
    priv/UploadManager.h \
    Builder.h \
    priv/Constants.h \
    priv/Log.h \
    priv/Upload.h
