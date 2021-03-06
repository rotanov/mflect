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

#QMAKE_CXXFLAGS_RELEASE -= -O2
#gcc: QMAKE_CXXFLAGS_RELEASE += -O0
#msvc: QMAKE_CXXFLAGS_RELEASE += /Od

DESTDIR = ../../bin

INCLUDEPATH += ../../ \
  ../../extras

CONFIG(debug, debug|release) {

    DEFINES += \
        _DEBUG \

    TARGET = cast-performance-debug

} else {

    TARGET = cast-performance-release

  msvc {
    QMAKE_CXXFLAGS += /Zi
    QMAKE_LFLAGS += /DEBUG
  }

}

SOURCES += main.cpp \
    ../../extras/test_classes.cpp

HEADERS += \
    ../../extras/test_classes.hpp

