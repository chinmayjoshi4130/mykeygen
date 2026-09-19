# ============================================================
# COMPILER DETECTION
# ============================================================

CXX ?=

ifeq ($(strip $(CXX)),)

	ifneq ($(shell command -v clang++ 2>/dev/null),)
		CXX := clang++

	else ifneq ($(shell command -v g++ 2>/dev/null),)
		CXX := g++

	else ifneq ($(shell command -v c++ 2>/dev/null),)
		CXX := c++

	else
		$(error No C++ compiler found. Install clang++, g++, or another C++ compiler)

	endif

endif


# ============================================================
# TARGET
# ============================================================

# Optional cross-compilation target.
#
# Example:
#   make TARGET_FLAGS="--target=x86_64-w64-windows-gnu"
#
TARGET_FLAGS ?=

CXX_TARGET := $(shell $(CXX) $(TARGET_FLAGS) -dumpmachine 2>/dev/null)


# ============================================================
# ARCHIVER DETECTION
# ============================================================

AR ?=

ifeq ($(strip $(AR)),)

	ifneq ($(shell command -v llvm-ar 2>/dev/null),)
		AR := llvm-ar

	else ifneq ($(shell command -v ar 2>/dev/null),)
		AR := ar

	else
		$(error No archiver found. Install llvm-ar or ar)

	endif

endif


# ============================================================
# FLAGS
# ============================================================

CXXFLAGS := \
	-std=c++17 \
	-Wall \
	-Wextra \
	-pedantic \
	-O2 \
	$(TARGET_FLAGS)

CPPFLAGS := \
	-Iinclude \
	-Isrc


# ============================================================
# PROJECT
# ============================================================

TARGET := mykeygen
LIBRARY := libmykeygen.a

SRC_DIR := src
BUILD_DIR := build
DEV_DIR := $(BUILD_DIR)/dev
INCLUDE_DIR := include

PREFIX ?= /usr/local
BINDIR := $(PREFIX)/bin
INCLUDEDIR := $(PREFIX)/include
LIBDIR := $(PREFIX)/lib


# ============================================================
# PLATFORM
# ============================================================

ifneq ($(findstring windows,$(CXX_TARGET)),)

	PLATFORM := windows
	RANDOM_SOURCE := $(SRC_DIR)/random_windows.cpp
	LIBS := -lbcrypt
	EXE_SUFFIX := .exe

else ifeq ($(OS),Windows_NT)

	PLATFORM := windows
	RANDOM_SOURCE := $(SRC_DIR)/random_windows.cpp
	LIBS := -lbcrypt
	EXE_SUFFIX := .exe

else

	PLATFORM := posix
	RANDOM_SOURCE := $(SRC_DIR)/random_posix.cpp
	LIBS :=
	EXE_SUFFIX :=

endif


# ============================================================
# SOURCES
# ============================================================

LIB_SOURCES := \
	$(SRC_DIR)/mykeygen.cpp \
	$(SRC_DIR)/random.cpp \
	$(RANDOM_SOURCE) \
	$(SRC_DIR)/charset.cpp \
	$(SRC_DIR)/generators.cpp

CLI_SOURCES := \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/options.cpp \
	$(SRC_DIR)/output.cpp


# ============================================================
# OBJECTS
# ============================================================

LIB_OBJECTS := \
	$(LIB_SOURCES:$(SRC_DIR)/%.cpp=$(DEV_DIR)/%.o)

CLI_OBJECTS := \
	$(CLI_SOURCES:$(SRC_DIR)/%.cpp=$(DEV_DIR)/%.o)

OBJECTS := \
	$(LIB_OBJECTS) \
	$(CLI_OBJECTS)

DEPFILES := $(OBJECTS:.o=.d)


# ============================================================
# OUTPUT ARTIFACTS
# ============================================================

CLI_TARGET := $(BUILD_DIR)/$(TARGET)$(EXE_SUFFIX)
LIB_TARGET := $(BUILD_DIR)/$(LIBRARY)


# ============================================================
# PHONY
# ============================================================

.PHONY: \
	all \
	library \
	clean \
	rebuild \
	install \
	uninstall \
	info


# ============================================================
# DEFAULT
# ============================================================

all: $(CLI_TARGET) $(LIB_TARGET)


# ============================================================
# LIBRARY
# ============================================================

library: $(LIB_TARGET)

$(LIB_TARGET): $(LIB_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	@echo "AR   $@"
	$(AR) rcs $@ $^


# ============================================================
# CLI
# ============================================================

$(CLI_TARGET): $(LIB_OBJECTS) $(CLI_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	@echo "LD   $@"
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@


# ============================================================
# COMPILATION
# ============================================================

$(DEV_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(DEV_DIR)
	@echo "CXX  $<"
	$(CXX) \
		$(CPPFLAGS) \
		$(CXXFLAGS) \
		-MMD \
		-MP \
		-MF $(DEV_DIR)/$*.d \
		-c $< \
		-o $@


# ============================================================
# DEPENDENCIES
# ============================================================

-include $(DEPFILES)


# ============================================================
# INSTALL
# ============================================================

install: all

ifeq ($(PLATFORM),windows)

	@echo "Installing mykeygen on Windows..."

	@if not exist "$(BINDIR)" mkdir "$(BINDIR)"
	@if not exist "$(LIBDIR)" mkdir "$(LIBDIR)"
	@if not exist "$(INCLUDEDIR)\mykeygen" mkdir "$(INCLUDEDIR)\mykeygen"

	@copy /Y "$(CLI_TARGET)" \
		"$(BINDIR)\$(TARGET)$(EXE_SUFFIX)"

	@copy /Y "$(LIB_TARGET)" \
		"$(LIBDIR)\$(LIBRARY)"

	@copy /Y "$(INCLUDE_DIR)\mykeygen\mykeygen.hpp" \
		"$(INCLUDEDIR)\mykeygen\mykeygen.hpp"

else

	@mkdir -p "$(BINDIR)"
	@mkdir -p "$(LIBDIR)"
	@mkdir -p "$(INCLUDEDIR)/mykeygen"

	@cp "$(CLI_TARGET)" \
		"$(BINDIR)/$(TARGET)"

	@cp "$(LIB_TARGET)" \
		"$(LIBDIR)/$(LIBRARY)"

	@cp "$(INCLUDE_DIR)/mykeygen/mykeygen.hpp" \
		"$(INCLUDEDIR)/mykeygen/mykeygen.hpp"

endif


# ============================================================
# UNINSTALL
# ============================================================

uninstall:

ifeq ($(PLATFORM),windows)

	@del /Q "$(BINDIR)\$(TARGET)$(EXE_SUFFIX)" 2>nul || exit 0
	@del /Q "$(LIBDIR)\$(LIBRARY)" 2>nul || exit 0
	@del /Q "$(INCLUDEDIR)\mykeygen\mykeygen.hpp" 2>nul || exit 0
	@rmdir "$(INCLUDEDIR)\mykeygen" 2>nul || exit 0

else

	@rm -f "$(BINDIR)/$(TARGET)"
	@rm -f "$(LIBDIR)/$(LIBRARY)"
	@rm -f "$(INCLUDEDIR)/mykeygen/mykeygen.hpp"
	@rmdir "$(INCLUDEDIR)/mykeygen" 2>/dev/null || true

endif


# ============================================================
# CLEAN
# ============================================================

clean:

ifeq ($(PLATFORM),windows)

	@if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)"

else

	@rm -rf "$(BUILD_DIR)"

endif


# ============================================================
# REBUILD
# ============================================================

rebuild: clean all


# ============================================================
# INFORMATION
# ============================================================

info:
	@echo "mykeygen build configuration"
	@echo "-----------------------------"
	@echo "Platform       : $(PLATFORM)"
	@echo "Compiler       : $(CXX)"
	@echo "Compiler target: $(CXX_TARGET)"
	@echo "Archiver       : $(AR)"
	@echo "Random backend : $(RANDOM_SOURCE)"
	@echo "Libraries      : $(LIBS)"
	@echo "C++ standard   : C++17"
	@echo "Build directory: $(BUILD_DIR)"
	@echo "Development dir: $(DEV_DIR)"
	@echo "Install prefix : $(PREFIX)"