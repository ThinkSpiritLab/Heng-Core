CXX ?= g++



COMMON_FLAGS += $(EXTRA) -O2\
	-Iinclude \
	-std=c++2a \
	-fPIE \
	-Wall -Wextra -Werror \

LD_FLAGS += --static -lstdc++fs
SRC_DIR = src
BUILD_DIR = build

TEST_SRC = test_src
TEST_BUILD = test_build

INSTALL_DIR = /usr/loacl/bin
BIN = hc

TEST_SRCS_CXX = $(wildcard $(TEST_SRC)/*.cpp)
TEST_BIN = $(patsubst $(TEST_SRC)/%.cpp,$(TEST_BUILD)/%.o,$(TEST_SRCS_CXX))

SRCS_CXX = $(wildcard $(SRC_DIR)/*.cpp)
SRCS_HXX = $(patsubst %.cpp,%.hpp,$(SRCS_CXX))

OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS_CXX))

LIBS = 

.PHONY:all clean install testbin

all:$(BUILD_DIR) $(BIN) 



testbin:$(TEST_BUILD) $(TEST_BIN)

$(TEST_BUILD):
	mkdir $@

$(BUILD_DIR):
	mkdir $@

$(BIN):$(OBJ) $(SRCS_CXX) $(SRCS_HXX)
	$(CXX) -o $(BIN) $(OBJ) $(LIBS) $(LD_FLAGS)

$(TEST_BUILD)/%.o:$(TEST_SRC)/%.cpp
	$(CXX) $(COMMON_FLAGS) $(CPPFLAGS) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o:$(SRC_DIR)/%.cpp $(SRC_DIR)/%.hpp
	$(CXX) $(COMMON_FLAGS) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/%.cpp:$(SRC_DIR)/%.hpp

install:
	cp $(BIN) $(INSTALL_DIR)

clean :
	-rm $(BUILD_DIR) -rf
	-rm $(BIN) -f
	-rm $(TEST_BUILD) -rf



