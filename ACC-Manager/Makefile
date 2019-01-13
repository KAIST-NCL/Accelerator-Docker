.PHONY:all

INC_DIR := include
SRC_DIR := src
OUT_DIR := out
BUILD_DIR := build
TEST_DIR := test
T_PARSER := t_parser
TARGET := acc-manager
PROTO_NAME := device
SRCS := $(wildcard src/*)
SRCS := $(notdir $(filter-out src/$(TARGET).cc, $(SRCS)))
OBJS := $(SRCS:.cc=.o)

FLAGS := -I $(INC_DIR) -I /usr/local/include -L/usr/local/lib -L$(BUILD_DIR) -lpci -lprotobuf -pthread

all: $(clean) $(TARGET)

$(TARGET):
	@if [ ! -d "$(BUILD_DIR)" ]; then mkdir $(BUILD_DIR); fi
	protoc -I=$(OUT_DIR) --cpp_out=$(BUILD_DIR) $(OUT_DIR)/$(PROTO_NAME).proto
	mv $(BUILD_DIR)/*.h $(INC_DIR)/
	mv $(BUILD_DIR)/*.cc $(SRC_DIR)/
	g++ $(SRC_DIR)/* -D_GNU_SOURCE $(FLAGS) -o $(OUT_DIR)/$@ -std=c++11

cont-ubuntu16.04:
	@if [ ! -d "$(OUT_DIR)" ]; then mkdir $(OUT_DIR); fi
	docker build --build-arg UID=$(shell id -u) --build-arg GID=$(shell id -g) -t "acc/manager/ubuntu:16.04" -f Make.Dockerfile .
	docker run --rm -v $(CURDIR)/$(OUT_DIR):/dist:Z -u $(shell id -u):$(shell id -g) "acc/manager/ubuntu:16.04" /bin/bash -c "cp /ACC-Manager/out/$(TARGET) /dist && cp /usr/local/lib/libprotobuf.so.17 /dist"

$(T_PARSER): $(clean) $(OBJS)
	g++ $(TEST_DIR)/tb_device-detector.cc $(addprefix $(BUILD_DIR)/, $^)  $(FLAGS) -o $(OUT_DIR)/$@ -std=c++11

%.o: 
	g++ -c -o $(BUILD_DIR)/$@ $(SRC_DIR)/$*.cc $(FLAGS) -std=c++11

install:
	cp $(OUT_DIR)/$(TARGET) /usr/bin/$(TARGET)
	if [ -f "$(OUT_DIR)/libprotobuf.so.17" ]; then cp $(OUT_DIR)/libprotobuf.so.17 /usr/local/lib/libprotobuf.so.17; fi

clean:
	rm -rf $(BUILD_DIR)
	if [ -f "$(OUT_DIR)/$(TARGET)" ]; then rm $(OUT_DIR)/$(TARGET); fi
	if [ -f "$(OUT_DIR)/libprotobuf.so.17" ]; then rm $(OUT_DIR)/libprotobuf.so.17; fi
	if [ -f "$(INC_DIR)/$(PROTO_NAME).pb.h" ]; then rm $(INC_DIR)/$(PROTO_NAME).pb.h; fi
	if [ -f "$(SRC_DIR)/$(PROTO_NAME).pb.cc" ]; then rm $(SRC_DIR)/$(PROTO_NAME).pb.cc; fi
	if [ "$(shell docker images -q acc/manager/ubuntu:16.04 2> /dev/null)" != "" ]; then docker rmi "acc/manager/ubuntu:16.04"; fi
