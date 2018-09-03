.PHONY:all

INC_DIR := include
SRC_DIR := src
OUT_DIR := out
BUILD_DIR := build
TEST_DIR := test
T_PARSER := t_parser
TARGET := acc-manager
SRCS := $(wildcard src/*)
SRCS := $(notdir $(filter-out src/$(TARGET).cc, $(SRCS)))
OBJS := $(SRCS:.cc=.o)

FLAGS := -I $(INC_DIR) -I /usr/local/include -L/usr/local/lib -L$(BUILD_DIR) -lpci -lprotobuf

all: $(clean) $(TARGET)

$(TARGET):
	g++ $(SRC_DIR)/* -D_GNU_SOURCE $(FLAGS) -o $(OUT_DIR)/$@ -std=c++11

$(T_PARSER): $(clean) $(OBJS)
	g++ $(TEST_DIR)/tb_device-detector.cc $(addprefix $(BUILD_DIR)/, $^)  $(FLAGS) -o $(OUT_DIR)/$@ -std=c++11

%.o: 
	g++ -c -o $(BUILD_DIR)/$@ $(SRC_DIR)/$*.cc $(FLAGS) -std=c++11

install:
	cp $(OUT_DIR)/acc-manager /usr/bin/$(TARGET)

clean:
	rm $(BUILD_DIR)/*
	rm $(OUT_DIR)/$(TARGET)
	rm /usr/bin/$(TARGET)
