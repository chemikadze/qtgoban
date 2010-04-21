#############################################################################
# Makefile for building: qtgoban
# Generated by qmake (2.01a) (Qt 4.5.3) on: ?? ???. 21 13:36:45 2010
# Project:  qtgoban.pro
# Template: app
# Command: /usr/bin/qmake-qt4 -spec /usr/share/qt4/mkspecs/linux-g++ -unix CONFIG+=debug -o Makefile qtgoban.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DDEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -g -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -g -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I.
LINK          = g++
LFLAGS        = 
LIBS          = $(SUBLIBS)  -L/usr/lib -lQtGui -lQtCore -lpthread
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake-qt4
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -sf
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = sgftree.cpp \
		sgfgame.cpp \
		sgfvariant.cpp \
		main.cpp \
		gametree.cpp \
		playerinput.cpp \
		abstractboard.cpp moc_sgfgame.cpp \
		moc_gametree.cpp \
		moc_playerinput.cpp \
		moc_abstractboard.cpp
OBJECTS       = sgftree.o \
		sgfgame.o \
		sgfvariant.o \
		main.o \
		gametree.o \
		playerinput.o \
		abstractboard.o \
		moc_sgfgame.o \
		moc_gametree.o \
		moc_playerinput.o \
		moc_abstractboard.o
DIST          = /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		qtgoban.pro
QMAKE_TARGET  = qtgoban
DESTDIR       = 
TARGET        = qtgoban

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: qtgoban.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/libQtGui.prl \
		/usr/lib/libQtCore.prl
	$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ -unix CONFIG+=debug -o Makefile qtgoban.pro
/usr/share/qt4/mkspecs/common/g++.conf:
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/debug.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/libQtGui.prl:
/usr/lib/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ -unix CONFIG+=debug -o Makefile qtgoban.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/qtgoban1.0.0 || $(MKDIR) .tmp/qtgoban1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/qtgoban1.0.0/ && $(COPY_FILE) --parents sgftree.h sgfgame.h sgfvariant.h common.h gametree.h playerinput.h abstractboard.h .tmp/qtgoban1.0.0/ && $(COPY_FILE) --parents sgftree.cpp sgfgame.cpp sgfvariant.cpp main.cpp gametree.cpp playerinput.cpp abstractboard.cpp .tmp/qtgoban1.0.0/ && (cd `dirname .tmp/qtgoban1.0.0` && $(TAR) qtgoban1.0.0.tar qtgoban1.0.0 && $(COMPRESS) qtgoban1.0.0.tar) && $(MOVE) `dirname .tmp/qtgoban1.0.0`/qtgoban1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/qtgoban1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_sgfgame.cpp moc_gametree.cpp moc_playerinput.cpp moc_abstractboard.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_sgfgame.cpp moc_gametree.cpp moc_playerinput.cpp moc_abstractboard.cpp
moc_sgfgame.cpp: common.h \
		sgftree.h \
		sgfvariant.h \
		sgfgame.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) sgfgame.h -o moc_sgfgame.cpp

moc_gametree.cpp: sgfgame.h \
		common.h \
		sgftree.h \
		sgfvariant.h \
		gametree.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) gametree.h -o moc_gametree.cpp

moc_playerinput.cpp: common.h \
		playerinput.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) playerinput.h -o moc_playerinput.cpp

moc_abstractboard.cpp: playerinput.h \
		common.h \
		abstractboard.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) abstractboard.h -o moc_abstractboard.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean 

####### Compile

sgftree.o: sgftree.cpp sgftree.h \
		sgfvariant.h \
		common.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o sgftree.o sgftree.cpp

sgfgame.o: sgfgame.cpp sgfgame.h \
		common.h \
		sgftree.h \
		sgfvariant.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o sgfgame.o sgfgame.cpp

sgfvariant.o: sgfvariant.cpp sgfvariant.h \
		common.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o sgfvariant.o sgfvariant.cpp

main.o: main.cpp sgfvariant.h \
		common.h \
		sgftree.h \
		sgfgame.h \
		gametree.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

gametree.o: gametree.cpp gametree.h \
		sgfgame.h \
		common.h \
		sgftree.h \
		sgfvariant.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gametree.o gametree.cpp

playerinput.o: playerinput.cpp playerinput.h \
		common.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o playerinput.o playerinput.cpp

abstractboard.o: abstractboard.cpp abstractboard.h \
		playerinput.h \
		common.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o abstractboard.o abstractboard.cpp

moc_sgfgame.o: moc_sgfgame.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_sgfgame.o moc_sgfgame.cpp

moc_gametree.o: moc_gametree.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_gametree.o moc_gametree.cpp

moc_playerinput.o: moc_playerinput.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_playerinput.o moc_playerinput.cpp

moc_abstractboard.o: moc_abstractboard.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_abstractboard.o moc_abstractboard.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

