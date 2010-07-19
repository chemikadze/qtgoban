HEADERS += sgftree.h \
    sgfgame.h \
    sgfvariant.h \
    common.h \
    gametree.h \
    playerinput.h \
    abstractboard.h \
    board.h \
    commentview.h \
    processmatrix.h \
    mainwindow.h \
    tabwidget.h \
    newgamedialog.h \
    choosetoolbutton.h \
    moveattributewidget.h
SOURCES += sgftree.cpp \
    sgfgame.cpp \
    sgfvariant.cpp \
    main.cpp \
    gametree.cpp \
    playerinput.cpp \
    abstractboard.cpp \
    board.cpp \
    common.cpp \
    commentview.cpp \
    mainwindow.cpp \
    tabwidget.cpp \
    newgamedialog.cpp \
    choosetoolbutton.cpp \
    moveattributewidget.cpp
OTHER_FILES += output.sgf \
    input.sgf
OBJECTS_DIR = ./obj
MOC_DIR = ./moc
debug { 
    DEFINES += DEBUG
    CONFIG += warn_on
}
