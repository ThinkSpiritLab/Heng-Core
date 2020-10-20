CXX ?= g++

COMMON_FLAGS += -O2 -c\
	-Iinclude \
	-std=c++2a \
	-fPIE \
	-Wall -Wextra -Werror \

LD_FLAGS += --static -lstdc++fs
SRC_DIR = src
BUILD_DIR = build
INSTALL_DIR = /usr/loacl/bin
BIN = hc

SRCS_CXX = $(wildcard $(SRC_DIR)/*.cpp)
SRCS_HXX = $(patsubst %.cpp,%.hpp,$(SRCS_CXX))

OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS_CXX))

LIBS = 

.PHONY:all clean install

all:$(BIN)

$(BIN):$(OBJ) $(SRCS_CXX) $(SRCS_HXX)
	$(CXX) -o $(BIN) $(OBJ) $(LIBS) $(LD_FLAGS)

$(BUILD_DIR)/%.o:$(SRC_DIR)/%.cpp $(SRC_DIR)/%.hpp
	$(CXX) $(COMMON_FLAGS) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/%.cpp:$(SRC_DIR)/%.hpp

install:
	cp $(BIN) $(INSTALL_DIR)

clean :
	-rm $(BUILD_DIR)/* -rf
	-rm $(BIN) -f



