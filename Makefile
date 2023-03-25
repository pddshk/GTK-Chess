CC		= clang
STD		?= c17
GTK		= gtk+-3.0
RSVG	= librsvg-2.0
GIO		= gio-unix-2.0 gio-2.0
PKGCONF	= $(shell which pkg-config)
CFLAGS	+= -Wall -std=$(STD) -O3 `$(PKGCONF) --cflags $(GTK) $(RSVG) $(GIO)`
LDFLAGS	+= `$(PKGCONF) --libs $(GTK) $(RSVG) $(GIO)` -lm
CHECKFLAGS += -Wextra -pedantic -fsyntax-only
CPPCHECKFLAGS += --enable=all -x c --error-exitcode=1 --std=c11 --suppress=missingIncludeSystem --suppress=unusedFunction
GCC = $(shell which gcc)
CLANG = $(shell which clang)
CPPCHECK = $(shell which cppcheck)

OBJDIR	= obj
SRCDIR	= src
NAMES   = main board state rules gtkchessapp
OBJECTS	= $(addprefix $(OBJDIR)/, $(addsuffix .o, $(NAMES)))
SOURCES = $(addprefix $(SRCDIR)/, $(addsuffix .c, $(NAMES)))
HEADERS = $(addprefix $(SRCDIR)/, $(addsuffix .h, $(NAMES)))

GLIB_COMPILE_RESOURCES = $(shell $(PKGCONF) --variable=glib_compile_resources gio-2.0)
BUILT_SRC = obj/resources.c

OBJECTS += $(BUILT_SRC:.c=.o)

GRESOURCE = src/gtkchessapp.gresource.xml
UI = src/window.glade

all: prepare engine_manager $(OBJECTS)
	$(CC) $(CFLAGS) -rdynamic -o GTKChess $(OBJECTS) $(LDFLAGS)
	gio set -t string GTKChess metadata::custom-icon file://$(PWD)/src/textures/classic/WKnight.svg

engine_manager: $(OBJDIR)/engine_manager.o
	$(CC) $(CFLAGS) -o engine_manager $< $(LDFLAGS) -pthread

$(BUILT_SRC): $(GRESOURCE) $(UI)
	$(GLIB_COMPILE_RESOURCES) $(GRESOURCE) --target=$@ --sourcedir=$(SRCDIR) --generate-source

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

check: check_sources check_headers

check_sources:
	$(GCC) $(CFLAGS) $(CHECKFLAGS) $(SOURCES)
	$(CLANG) $(CFLAGS) $(CHECKFLAGS) $(SOURCES)
	$(CPPCHECK) $(CPPCHECKFLAGS) $(SOURCES)

check_headers:
	$(CPPCHECK) $(CPPCHECKFLAGS) $(HEADERS)
