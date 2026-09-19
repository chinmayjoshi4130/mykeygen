CXX := clang++

CXXFLAGS := \
	-std=c++17 \
	-Wall \
	-Wextra \
	-pedantic \
	-O2

CPPFLAGS := -Iinclude -Isrc

TARGET := mykeygen
LIBRARY := libmykeygen.a

SRC_DIR := src
BUILD_DIR := build
INCLUDE_DIR := include

PREFIX ?= /usr/local
BINDIR := $(PREFIX)/bin
INCLUDEDIR := $(PREFIX)/include
LIBDIR := $(PREFIX)/lib

LIB_SOURCES := \
	$(SRC_DIR)/mykeygen.cpp \
	$(SRC_DIR)/random.cpp \
	$(SRC_DIR)/charset.cpp \
	$(SRC_DIR)/generators.cpp

CLI_SOURCES := \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/options.cpp \
	$(SRC_DIR)/output.cpp

LIB_OBJECTS := \
	$(LIB_SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

CLI_OBJECTS := \
	$(CLI_SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

OBJECTS := $(LIB_OBJECTS) $(CLI_OBJECTS)

DEPFILES := $(OBJECTS:.o=.d)

CLI_TARGET := $(BUILD_DIR)/$(TARGET)
LIB_TARGET := $(BUILD_DIR)/$(LIBRARY)

.PHONY: all library clean rebuild install uninstall

all: $(CLI_TARGET) $(LIB_TARGET)

library: $(LIB_TARGET)

# ------------------------------------------------------------
# CLI
# ------------------------------------------------------------

$(CLI_TARGET): $(LIB_OBJECTS) $(CLI_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	@echo "LD   $@"
	$(CXX) $(CXXFLAGS) $^ -o $@

# ------------------------------------------------------------
# Static library
# ------------------------------------------------------------

$(LIB_TARGET): $(LIB_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	@echo "AR   $@"
	ar rcs $@ $^

# ------------------------------------------------------------
# Compile
# ------------------------------------------------------------

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	@echo "CXX  $<"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) \
		-MMD \
		-MP \
		-MF $(BUILD_DIR)/$*.d \
		-c $< \
		-o $@

-include $(DEPFILES)

# ------------------------------------------------------------
# Install
# ------------------------------------------------------------

install: all
	@mkdir -p $(BINDIR)
	@mkdir -p $(LIBDIR)
	@mkdir -p $(INCLUDEDIR)/mykeygen

	@cp $(CLI_TARGET) \
		$(BINDIR)/$(TARGET)

	@cp $(LIB_TARGET) \
		$(LIBDIR)/$(LIBRARY)

	@cp $(INCLUDE_DIR)/mykeygen/mykeygen.hpp \
		$(INCLUDEDIR)/mykeygen/mykeygen.hpp

	@echo "Installed $(TARGET) to $(BINDIR)/$(TARGET)"
	@echo "Installed $(LIBRARY) to $(LIBDIR)/$(LIBRARY)"
	@echo "Installed header to $(INCLUDEDIR)/mykeygen/mykeygen.hpp"

# ------------------------------------------------------------
# Uninstall
# ------------------------------------------------------------

uninstall:
	@rm -f $(BINDIR)/$(TARGET)
	@rm -f $(LIBDIR)/$(LIBRARY)
	@rm -f $(INCLUDEDIR)/mykeygen/mykeygen.hpp
	@rmdir $(INCLUDEDIR)/mykeygen 2>/dev/null || true

	@echo "Uninstalled $(TARGET)"
	@echo "Uninstalled $(LIBRARY)"
	@echo "Uninstalled mykeygen header"

# ------------------------------------------------------------
# Cleanup
# ------------------------------------------------------------

clean:
	@rm -rf $(BUILD_DIR)

rebuild: clean all