.NOTPARALLEL:
.PHONY:all

SOURCES := $(shell find . 2>&1 | grep -E '.*\.(c|h|go)$$')

OUT_DIR := ./out
BIN_NAME := fpga-runtime-hook

all: $(SOURCES)
	@if [ ! -d "$(OUT_DIR)" ]; then mkdir $(OUT_DIR); fi	
	go build -ldflags "-s -w -v" -o $(OUT_DIR)/$(BIN_NAME) .

install: 
	sudo cp ./${BIN_NAME} /usr/bin/${BIN_NAME}

clean:
	rm -rf $(OUT_DIR)
