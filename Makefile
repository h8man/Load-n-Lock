EMXX ?= em++
BUILD_DIR ?= build/web
TARGET_NAME ?= index
TARGET ?= $(BUILD_DIR)/$(TARGET_NAME).html
SHELL_FILE ?= shell.html
RAYLIB_WEB_DIR ?= raylib-web
RAYLIB_WEB_INCLUDE_DIR ?= $(RAYLIB_WEB_DIR)/include
RAYLIB_WEB_LIB ?= $(RAYLIB_WEB_DIR)/lib/libraylib.web.a

MAIN_SRC := Load\ \&\ Lock.cpp
SOURCES := \
    src/assets/AssetManager.cpp \
    src/audio/RaylibAudioPlayer.cpp \
    src/core/Application.cpp \
    src/game/Cutscene.cpp \
    src/game/GameLogic.cpp \
    src/input/RaylibInputHandler.cpp \
    src/renderer/RaylibRenderer.cpp

COMMON_FLAGS := \
    -std=c++17 \
    -DLOAD_AND_LOCK_USE_RAYLIB \
    -I. \
    -I$(RAYLIB_WEB_INCLUDE_DIR) \
    -Wall \
    -Wextra \
    -pedantic

EMFLAGS := \
    -sUSE_GLFW=3 \
    -sALLOW_MEMORY_GROWTH=1 \
    -sASYNCIFY \
    -sFORCE_FILESYSTEM=1 \
    --preload-file assets \
    --preload-file sprites.png \
    --preload-file L\&L.png \
    --shell-file $(SHELL_FILE)

DEBUG_FLAGS := -O0 -gsource-map -sASSERTIONS=2
RELEASE_FLAGS := -O3

.RECIPEPREFIX := >

.PHONY: all debug release clean serve

all: release

debug: CXXFLAGS := $(COMMON_FLAGS) $(DEBUG_FLAGS)
debug: $(TARGET)

release: CXXFLAGS := $(COMMON_FLAGS) $(RELEASE_FLAGS)
release: $(TARGET)

$(TARGET): $(SOURCES) $(SHELL_FILE)
> mkdir -p $(BUILD_DIR)
> cp L\&L.png $(BUILD_DIR)
> cp L\&L.ico $(BUILD_DIR)
> $(EMXX) $(MAIN_SRC) $(SOURCES) $(CXXFLAGS) $(EMFLAGS) $(RAYLIB_WEB_LIB) -o $(TARGET) 

clean:
> rm -rf $(BUILD_DIR)/*

serve: release
> emrun --no_browser --port 8080 $(TARGET)
