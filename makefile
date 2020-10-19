CXX ?= g++

COMMON_FLAGS += -O2 -c\
	-D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 \
	-std=c++2a \
	-fPIE \
	-Wall -Wextra -Werror \

LD_FLAGS += --static

SRC_DIR = src
BUILD_DIR = build
INSTALL_DIR = /usr/loacl/bin
BIN = hc

SRCS_CXX = hc.cpp
SRCS_HXX = $(patsubst %.cpp,%.hpp,$(SRCS_CXX))

_OBJ = $(patsubst %.cpp,%.o,$(SRCS_CXX))
OBJ = $(addprefix $(BUILD_DIR)/,$(_OBJ))

LIBS = 

.PHONY : all clean install

all : $(BIN)

$(BIN) : $(OBJ)
	$(CXX) -o $(BIN) $(OBJ) $(LIBS) $(LDFLAGS)


$(BUILD_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CXX) $(COMMON_FLAGS) $(CPPFLAGS) $(CFLAGS) -c $^ -o $@

install:
	cp $(BIN) $(INSTALL_DIR)

clean :
	-rm $(BUILD_DIR)/* -rf
	-rm $(BIN) -f



