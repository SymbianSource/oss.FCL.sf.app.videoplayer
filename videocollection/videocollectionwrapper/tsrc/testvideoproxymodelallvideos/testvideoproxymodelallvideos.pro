TEMPLATE = app
TARGET = 
DEFINES     += BUILD_VIDEOCOLLECTION_DLL

DEPENDPATH += . \
    inc \
    src \
    stub/inc \
    stub/src
    
INCLUDEPATH += . \
    stub/inc \
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
HEADERS +=  inc/testvideoproxymodelallvideos.h \
            stub/inc/filterproxytester.h \
            stub/inc/videoproxymodelgeneric.h \
            stub/inc/videolistdatamodel.h \
            ../../inc/videoproxymodelallvideos.h
    
SOURCES += src/testvideoproxymodelallvideos.cpp \
           stub/src/filterproxytester.cpp \
           stub/src/videoproxymodelgeneric.cpp \
           stub/src/videolistdatamodel.cpp \
           ../../src/videoproxymodelallvideos.cpp
