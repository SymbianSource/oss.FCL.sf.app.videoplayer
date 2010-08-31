######################################################################
# Automatically generated by qmake (2.01a) Tue 5. May 13:25:42 2009
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += . inc src 
              
INCLUDEPATH += . \
               ../../../inc \ 
               /epoc32/include/domain \
               /epoc32/include/domain/middleware \
               /epoc32/include/domain/applications \
               /epoc32/include/osextensions \
               /epoc32/include/middleware \
               /epoc32/include/osextensions/stdapis/stlport \
               /VideoApp_Domain/video/videofeeds/inc

               
CONFIG += qtestlib \
          Hb \
          symbian_test

LIBS += -lmpxcommon.dll -lflogger.dll -lestor.dll \
        -lfbscli.dll -lbitgdi.dll -lgdi.dll -lxqutils.dll

# Input
HEADERS += stub/inc/metadatasignalspy.h \
           stub/inc/videolistdatamodel.h \
           stub/inc/videocollectionwrapper.h \
           stub/inc/videocollectionutils.h \
           stub/inc/videodatacontainer.h \
           stub/inc/videocollectionclient.h \
           ../../inc/videodatasignalreceiver.h \
           ../../inc/videolistdatamodel_p.h \           
           inc/testvideomodel_p.h \
           inc/mediaobjectfactory.h \           
           stub/inc/videothumbnaildata.h \
           
SOURCES += stub/src/videocollectionwrapper.cpp \
           stub/src/videocollectionutils.cpp \
           stub/src/videodatacontainer.cpp \
           stub/src/videocollectionclient.cpp \
           stub/src/videolistdatamodel.cpp \
           ../../src/videolistdatamodel_p.cpp \
           src/testvideomodel_p.cpp \
           src/mediaobjectfactory.cpp \           
           stub/src/videothumbnaildata.cpp \
