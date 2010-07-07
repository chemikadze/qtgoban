HEADERS += sgftree.h \
    sgfgame.h \
    sgfvariant.h \
    common.h \
    gametree.h \
    playerinput.h \
    abstractboard.h \
    board.h \
    commentview.h \
    processmatrix.h
SOURCES += sgftree.cpp \
    sgfgame.cpp \
    sgfvariant.cpp \
    main.cpp \
    gametree.cpp \
    playerinput.cpp \
    abstractboard.cpp \
    board.cpp \
    common.cpp \
    commentview.cpp
OTHER_FILES += output.sgf \
    input.sgf
OBJECTS_DIR = ./obj
MOC_DIR = ./moc
debug {
	DEFINES += DEBUG
	CONFIG += warn_on
}
