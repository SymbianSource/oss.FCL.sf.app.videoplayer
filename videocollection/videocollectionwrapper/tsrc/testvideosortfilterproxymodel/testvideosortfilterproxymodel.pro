# #####################################################################
# Automatically generated by qmake (2.01a) Tue 5. May 13:25:42 2009
# #####################################################################
TEMPLATE = app
TARGET = 
DEFINES     += BUILD_VIDEOCOLLECTION_DLL
DEPENDPATH += . \
    inc \
    src
INCLUDEPATH += . \
    /epoc32/include/domain \
    /epoc32/include/domain/middleware \
    /epoc32/include/domain/applications \
    /epoc32/include/osextensions \
    /epoc32/include/middleware \
    /epoc32/include/osextensions/stdapis/stlport \
    ../../../inc

CONFIG += qtestlib \
    Hb \
    symbian_test
          
LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lgdi.dll

# Input
HEADERS +=  inc/testvideosortfilterproxymodel.h \
            stub/inc/filterproxytester.h \
            stub/inc/videolistdatamodel.h \
            stub/inc/videocollectionclient.h \
            stub/inc/videothumbnaildata.h \
            stub/inc/videocollectionwrapper.h \
            ../../inc/videosortfilterproxymodel.h \
            ../../../inc/videocollectionexport.h
    
SOURCES += src/testvideosortfilterproxymodel.cpp \
           stub/src/filterproxytester.cpp \
           stub/src/videolistdatamodel.cpp \
           stub/src/videocollectionclient.cpp \
           stub/src/videothumbnaildata.cpp \
           ../../src/videosortfilterproxymodel.cpp