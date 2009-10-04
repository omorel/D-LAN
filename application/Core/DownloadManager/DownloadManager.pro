# -------------------------------------------------
# Project created by QtCreator 2009-10-03T12:42:43
# -------------------------------------------------
QT += network
QT -= gui

DESTDIR = "output/debug"
MOC_DIR = ".tmp/debug"
OBJECTS_DIR = ".tmp/debug"

INCLUDEPATH += . \
    ../.. \
    ${PROTOBUF}/src
LIBS += -L${PROTOBUF}/src/.libs \
    -lprotobuf
TARGET = DownloadManager
TEMPLATE = lib
CONFIG += shared
SOURCES += priv/FileDownload.cpp \
    priv/DownloadManager.cpp \
    priv/Download.cpp \
    priv/DirDownload.cpp \
    priv/ChunkDownloader.cpp \
    priv/ChunkDownload.cpp \
    ../../Protos/common.pb.cc
HEADERS += IDownloadManager.h \
    IDownload.h \
    IChunkDownload.h \
    priv/FileDownload.h \
    priv/DownloadManager.h \
    priv/Download.h \
    priv/DirDownload.h \
    priv/ChunkDownloader.h \
    priv/ChunkDownload.h \
    ../../Protos/common.pb.h