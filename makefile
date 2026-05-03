CXX ?= clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wno-unused-parameter -O2

APP_NAME := snow
VERSION  := 0.1.0-alpha
ZIP_NAME := $(APP_NAME)-macos-$(VERSION).zip

BINDIR := dist/macos
LIBDIR := $(BINDIR)/lib
OBJDIR := dist/obj

INCLUDES = \
	-I./include \
	-I./include/ui \
	-I./scripts/include \
	-I./blueprints/include \
	-I./scene_builders/include \
	-I./terminal/include \
	-I./snowlang/include \
	-I./snowlang/commands \

export PKG_CONFIG_PATH := /opt/homebrew/lib/pkgconfig

SFML_CFLAGS := $(shell pkg-config --cflags sfml-graphics sfml-window sfml-system sfml-audio)
SFML_LIBS   := $(shell pkg-config --libs   sfml-graphics sfml-window sfml-system sfml-audio)

JSON_CFLAGS := $(shell pkg-config --cflags nlohmann_json 2>/dev/null)

CXXFLAGS += $(INCLUDES) $(SFML_CFLAGS) $(JSON_CFLAGS)
LDFLAGS  += $(SFML_LIBS) -Wl,-rpath,@executable_path/lib

SRC := $(shell find src scripts/src blueprints/src terminal/src scene_builders/src snowlang/src snowlang/commands -name '*.cpp')
OBJ := $(SRC:%.cpp=$(OBJDIR)/%.o)

all: $(APP_NAME)

dist: $(BINDIR)/$(APP_NAME)

$(APP_NAME): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

$(BINDIR)/$(APP_NAME): $(OBJ)
	mkdir -p $(BINDIR)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

bundle-libs:
	mkdir -p $(LIBDIR)

	# SFML
	cp /opt/homebrew/opt/sfml@2.6/lib/libsfml-*.2.6.dylib $(LIBDIR)

	# Audio deps
	cp /opt/homebrew/opt/openal-soft/lib/libopenal*.dylib $(LIBDIR)
	cp /opt/homebrew/opt/libogg/lib/libogg*.dylib $(LIBDIR)
	cp /opt/homebrew/opt/libvorbis/lib/libvorbis*.dylib $(LIBDIR)
	cp /opt/homebrew/opt/flac/lib/libFLAC*.dylib $(LIBDIR)
	cp /opt/homebrew/opt/libsndfile/lib/libsndfile*.dylib $(LIBDIR)

	# Graphics deps
	cp /opt/homebrew/opt/freetype/lib/libfreetype*.dylib $(LIBDIR)
	cp /opt/homebrew/opt/libpng/lib/libpng*.dylib $(LIBDIR)
	cp /opt/homebrew/opt/zlib/lib/libz*.dylib $(LIBDIR)

fix-libs:
	@for lib in $(LIBDIR)/*.dylib; do \
		echo "Fixing ID $$lib"; \
		install_name_tool -id @rpath/$$(basename $$lib) $$lib; \
	done

fix-lib-deps:
	@for lib in $(LIBDIR)/*.dylib; do \
		otool -L $$lib | grep /opt/homebrew | awk '{print $$1}' | while read dep; do \
			echo "Rewriting $$dep in $$lib"; \
			install_name_tool -change $$dep @rpath/$$(basename $$dep) $$lib; \
		done; \
	done

fix-bin:
	@otool -L $(BINDIR)/$(APP_NAME) | grep /opt/homebrew | awk '{print $$1}' | while read dep; do \
		echo "Rewriting $$dep in binary"; \
		install_name_tool -change $$dep @rpath/$$(basename $$dep) $(BINDIR)/$(APP_NAME); \
	done

package: all bundle-libs fix-libs fix-lib-deps fix-bin
	cp -R assets $(BINDIR)/
	cp -R config $(BINDIR)/

sign:
	codesign --force --sign - $(LIBDIR)/*.dylib
	codesign --force --deep --sign - $(BINDIR)/$(APP_NAME)
	@echo "Ad-hoc signed all dylibs and binary"

zip: package sign
	rm -rf release
	mkdir -p release/$(APP_NAME)-macos-$(VERSION)

	# Copy app bundle
	cp -R dist/macos release/$(APP_NAME)-macos-$(VERSION)/

	# Copy root docs
	cp README.md HOW_TO_RUN.txt release/$(APP_NAME)-macos-$(VERSION)/

	# Create zip
	cd release && zip -r ../$(ZIP_NAME) $(APP_NAME)-macos-$(VERSION)

	rm -rf release
	@echo "Created $(ZIP_NAME)"

run: all
	./$(APP_NAME)

run-dist: all
	./$(BINDIR)/$(APP_NAME)

full-run: dist package bundle-libs sign run

clean:
	rm -rf $(OBJDIR) $(APP_NAME)

clean-dist:
	rm -rf dist release $(ZIP_NAME)

.PHONY: all clean run zip package bundle-libs fix-libs fix-lib-deps fix-bin sign full-run
