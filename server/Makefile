LDFLAGS := -pthread
# Check if the operating system is Windows
ifeq ($(OS),Windows_NT)
	CXX := -x86_64-w64-mingw32-g++
	LDLIBS := lib/libserialport.dll.a lib/libwebsockets.dll.a
# We can say it's Linux
else
	CXX  :=  -g++
	LDLIBS := -lwebsockets -lserialport
endif

CXXFLAGS := -std=c++20
INCLUDE = -I inc
BUILD    := ./obj
OBJ_DIR  := $(BUILD)/bin
APP_DIR  := $(BUILD)/app
TARGET   := raspflow-server

SRC      := $(wildcard src/*.cpp)

OBJECTS := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(APP_DIR)/$(TARGET)
	@echo "Build."

$(OBJ_DIR)/%.o: %.cpp
# Before we must create directory
#	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(APP_DIR)/$(TARGET): $(OBJECTS)
# Before we must create directory
#	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $(OBJECTS) $(LDLIBS) $(LDFLAGS)

.PHONY: all build clean debug release

build:
#	@mkdir -p $(APP_DIR)
#	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O4
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/src/*
