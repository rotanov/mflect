TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

gcc {
    QMAKE_CXXFLAGS += -std=c++11

    QMAKE_CXXFLAGS += -Wextra
    QMAKE_CXXFLAGS += -Werror

    # GCC handles C++11 class members inline
    # initialization wrong in context of warnings
    QMAKE_CXXFLAGS += -Wno-reorder

    QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
    QMAKE_CXXFLAGS += -Wno-unused-variable
}

DESTDIR = ../bin

INCLUDEPATH += ../

CONFIG(debug, debug|release) {

    DEFINES += \
        _DEBUG \

    TARGET = tests-debug

} else {

    TARGET = tests-release

}

SOURCES += main.cpp \
    test_classes.cpp \
    type_info_registration.cpp

HEADERS += \
    test_classes.hpp

