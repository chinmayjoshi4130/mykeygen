CXX := clang++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -O2

TARGET := mykeygen

SRC_DIR := src
BUILD_DIR := build

SOURCES := \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/random.cpp \
	$(SRC_DIR)/charset.cpp \
	$(SRC_DIR)/options.cpp \
	$(SRC_DIR)/generators.cpp \
	$(SRC_DIR)/output.cpp

OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPFILES := $(OBJECTS:.o=.d)

PREFIX ?= /usr/local
BINDIR := $(PREFIX)/bin

.PHONY: all clean rebuild install uninstall

all: $(BUILD_DIR)/$(TARGET)

# ------------------------------------------------------------
# Link
# ------------------------------------------------------------

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	@echo "LD   $@"
	$(CXX) $(CXXFLAGS) $^ -o $@

# ------------------------------------------------------------
# Compile
# ------------------------------------------------------------

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	@echo "CXX  $<"
	$(CXX) $(CXXFLAGS) \
		-MMD -MP \
		-MF $(BUILD_DIR)/$*.d \
		-c $< \
		-o $@

# Automatically generated header dependencies
-include $(DEPFILES)

# ------------------------------------------------------------
# Install
# ------------------------------------------------------------

install: $(BUILD_DIR)/$(TARGET)
	@mkdir -p $(BINDIR)
	@cp $(BUILD_DIR)/$(TARGET) $(BINDIR)/$(TARGET)
	@echo "Installed $(TARGET) to $(BINDIR)/$(TARGET)"

# ------------------------------------------------------------
# Uninstall
# ------------------------------------------------------------

uninstall:
	@rm -f $(BINDIR)/$(TARGET)
	@echo "Removed $(BINDIR)/$(TARGET)"

# ------------------------------------------------------------
# Cleanup
# ------------------------------------------------------------

clean:
	@rm -rf $(BUILD_DIR)

rebuild: clean all