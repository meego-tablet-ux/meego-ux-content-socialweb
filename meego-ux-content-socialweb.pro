#
# libsocialweb plugin for meego-ux-content
# 

VERSION = 0.1.1
TARGET = swplugin
TEMPLATE = lib

CONFIG += plugin link_pkgconfig debug

PKGCONFIG += libsocialweb-qt meego-ux-content

# use pkg-config paths for include in both g++ and moc
INCLUDEPATH += $$system(pkg-config --cflags meego-ux-content \
    | tr \' \' \'\\n\' | grep ^-I | cut -d 'I' -f 2-)

OBJECTS_DIR = .obj
MOC_DIR = .moc

SOURCES += \
    swplugin.cpp \
    swservicemodel.cpp \
    swmodel.cpp

HEADERS += \
    swplugin.h \
    swservicemodel.h \
    swmodel.h

target.path = $$[QT_INSTALL_PLUGINS]/MeeGo/Content
INSTALLS += target


#********* Translations *********
TRANSLATIONS += $${SOURCES} $${HEADERS} $${OTHER_FILES}
PROJECT_NAME = meego-ux-content-socialweb

dist.commands += rm -Rf $${PROJECT_NAME}-$${VERSION} &&
dist.commands += git clone . $${PROJECT_NAME}-$${VERSION} &&
dist.commands += rm -Rf $${PROJECT_NAME}-$${VERSION}/.git &&
dist.commands += mkdir -p $${PROJECT_NAME}-$${VERSION}/ts &&
dist.commands += lupdate $${TRANSLATIONS} -ts $${PROJECT_NAME}-$${VERSION}/ts/$${PROJECT_NAME}.ts &&
dist.commands += tar jcpvf $${PROJECT_NAME}-$${VERSION}.tar.bz2 $${PROJECT_NAME}-$${VERSION}
QMAKE_EXTRA_TARGETS += dist
#********* End Translations ******
