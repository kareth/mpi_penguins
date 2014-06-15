TARGET = main
ZOO_LIB = penguin.a

CXX = mpic++
LD = mpic++
AR = ar

CXX_FLAGS = -Wall -Wextra -std=c++0x
LD_FLAGS =
AR_FLAGS = rs

OBJ_DIR = bin
SRC_DIR = src

MAIN_SRCS = main.cpp
ZOO_LIB_SRCS = zoo.cpp communication.cpp message.cpp

MAIN_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(MAIN_SRCS:.cpp=.o)))
ZOO_LIB_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(ZOO_LIB_SRCS:.cpp=.o)))

INCLUDES = $(wildcard $(SRC_DIR)/*.h)

USER_MESSAGE = "THE WINTER IS COMING!"

.PHONY: all clean

all: $(OBJ_DIR) $(OBJ_DIR)/$(TARGET)
	@echo $(USER_MESSAGE)

$(OBJ_DIR)/$(TARGET): $(MAIN_OBJS) $(OBJ_DIR)/$(ZOO_LIB)
	$(LD) $(LD_FLAGS) -o $(OBJ_DIR)/$(TARGET) $^

$(OBJ_DIR)/$(ZOO_LIB): $(ZOO_LIB_OBJS)
	$(AR) $(AR_FLAGS) $(OBJ_DIR)/$(ZOO_LIB) $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDES)
	$(CXX) $(CXX_FLAGS) -o $@ -c $<

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	-rm -rf $(OBJ_DIR)/*.o
	-rm -rf $(OBJ_DIR)/*.a
	-rm -rf $(OBJ_DIR)/$(TARGET)

