.NOTPARALLEL:
.PHONY:all

SOURCES := $(shell find . 2>&1 | grep -E '.*\.(c|h|go)$$')

OUT_DIR := ./out
BIN_NAME := acc-runtime-hook

all: $(SOURCES)
	@if [ ! -d "$(OUT_DIR)" ]; then mkdir $(OUT_DIR); fi	
	go build -ldflags "-s -w -v" -o $(OUT_DIR)/$(BIN_NAME) .

cont-ubuntu16.04:
	@if [ ! -d "$(OUT_DIR)" ]; then mkdir $(OUT_DIR); fi
	docker build --build-arg UID=$(shell id -u) --build-arg GID=$(shell id -g) -t "acc/hook/ubuntu:16.04" -f Make.Dockerfile .
	docker run --rm -v $(CURDIR)/$(OUT_DIR):/dist:Z -u $(shell id -u):$(shell id -g) "acc/hook/ubuntu:16.04" /bin/bash -c "cp /ACC-Runtime-Hook/out/acc-runtime-hook /dist"

install: 
	sudo cp $(OUT_DIR)/${BIN_NAME} /usr/bin/${BIN_NAME}

clean:
	rm -rf $(OUT_DIR)
	if [ "$(shell docker images -q acc/hook/ubuntu:16.04 2> /dev/null)" != "" ]; then docker rmi "acc/hook/ubuntu:16.04"; fi
