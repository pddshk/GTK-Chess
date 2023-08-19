CC		= clang
STD		?= c17
GTK		= gtk+-3.0
RSVG	= librsvg-2.0
GIO		= gio-unix-2.0 gio-2.0
PKGCONF	?= $(shell which pkg-config)
CFLAGS	+= -Wall -Wextra -Wpedantic -Wno-overlength-strings -std=$(STD) -O3 `$(PKGCONF) --cflags $(GTK) $(RSVG) $(GIO)`
LDFLAGS	+= `$(PKGCONF) --libs $(GTK) $(RSVG) $(GIO)` -lm

OBJDIR	= obj
SRCDIR	= src
DATADIR = data
INCDIR  = include
DOCDIR	= docs
NAMES   = main board state rules gtkchessapp state_tree fen engine_controls notation game_info
OBJECTS	= $(addprefix $(OBJDIR)/, $(addsuffix .o, $(NAMES)))
SOURCES = $(addprefix $(SRCDIR)/, $(addsuffix .c, $(NAMES)))

CFLAGS += -I$(INCDIR)

CHECKFLAGS += -Wextra -pedantic -fsyntax-only
GCC = $(shell which gcc)
CLANG = $(shell which clang)
CPPCHECK = $(shell which cppcheck)
TEMPDIR = temp
CPPCHECKFLAGS += -x c --inline-suppr --error-exitcode=1
CPPCHECKFLAGS += --std=c11 --enable=style
CPPCHECKFLAGS += --cppcheck-build-dir=$(TEMPDIR)
CSOURCES ?= $(SOURCES) src/engine_manager.c

GLIB_COMPILE_RESOURCES = $(shell $(PKGCONF) --variable=glib_compile_resources gio-2.0)
BUILT_SRC = obj/resources.c

OBJECTS := $(BUILT_SRC:.c=.o) $(OBJECTS)

GRESOURCE = $(DATADIR)/gtkchessapp.gresource.xml
UI = $(DATADIR)/window.glade $(DATADIR)/selected.css

all: prepare engine_manager $(OBJECTS)
	$(CC) $(CFLAGS) -rdynamic -o GTKChess $(OBJECTS) $(LDFLAGS)
#	FIXME: `nix build` fails with error: 
#	gio: Setting attribute metadata::custom-icon not supported
#
#	gio set -t string GTKChess metadata::custom-icon file://$(PWD)/src/textures/classic/WKnight.svg

engine_manager: $(OBJDIR)/engine_manager.o
	$(CC) $(CFLAGS) -o engine_manager $< $(LDFLAGS) -pthread

$(BUILT_SRC): $(GRESOURCE) $(UI)
	$(GLIB_COMPILE_RESOURCES) $(GRESOURCE) --target=$(INCDIR)/UI.h --sourcedir=$(DATADIR) --generate-header
	$(GLIB_COMPILE_RESOURCES) $(GRESOURCE) --target=$@ --sourcedir=$(DATADIR) --generate-source

prepare:
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

cleaner: clean
	rm -f GTKChess engine_manager

clean:
	rm -f $(OBJDIR)/*

run:
	./GTKChess

check: prepare_check check_sources

prepare_check:
	mkdir -p $(TEMPDIR)

check_sources:
	$(GCC) $(CFLAGS) $(CHECKFLAGS) $(CSOURCES)
	$(CLANG) $(CFLAGS) $(CHECKFLAGS) $(CSOURCES)
	$(CPPCHECK) $(CPPCHECKFLAGS) $(CSOURCES)

clean_check:
	rm -f $(TEMPDIR)/*

clean_docs:
	rm -rf $(DOCDIR)/build

restore: cleaner clean_check clean_docs

docs: $(DOCDIR)/source
	cd $(DOCDIR) && $(MAKE) html && cd -

read: docs
	xdg-open $(DOCDIR)/build/html/index.html &> /dev/null
	
