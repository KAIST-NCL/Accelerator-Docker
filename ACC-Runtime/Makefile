.PHONY:all

TMP_GO_PATH := .tmp_go_path
OUT_DIR := ./out

TMP_GO_ABS := $(CURDIR)/$(TMP_GO_PATH)
ENV_GOPATH := env GOPATH=$(TMP_GO_ABS)

RUNC_REPO:=github.com/opencontainers/runc

TARGET := acc-runtime

all: $(clean) $(TARGET)

$(TARGET): 
	@if [ ! -d "$(TMP_GO_ABS)" ]; then mkdir $(TMP_GO_ABS); fi
	@echo Setting temp. GOPATH as [$(TMP_GO_ABS)]
	$(ENV_GOPATH) go get -d $(RUNC_REPO)/...
	patch -d$(TMP_GO_ABS)/src/$(RUNC_REPO) -p1 < acc-runtime.patch
	$(ENV_GOPATH) make -C $(TMP_GO_ABS)/src/$(RUNC_REPO)/ BUILDTAGS='apparmor seccomp'
	@if [ ! -d "$(OUT_DIR)" ]; then mkdir $(OUT_DIR); fi
	@mv $(TMP_GO_ABS)/src/$(RUNC_REPO)/$@ $(OUT_DIR)/$@

cont-ubuntu16.04:
	@if [ ! -d "$(OUT_DIR)" ]; then mkdir $(OUT_DIR); fi
	docker build --build-arg UID=$(shell id -u) --build-arg GID=$(shell id -g) -t "acc/runtime/ubuntu:16.04" -f Make.Dockerfile .
	docker run --rm -v $(CURDIR)/$(OUT_DIR):/dist:Z -u $(shell id -u):$(shell id -g) "acc/runtime/ubuntu:16.04" /bin/bash -c "cp /ACC-Runtime/out/acc-runtime /dist"

clean: 
	rm -rf $(TMP_GO_PATH)
	rm -rf $(OUT_DIR)
	if [ "$(shell docker images -q acc/runtime/ubuntu:16.04 2> /dev/null)" != "" ]; then docker rmi "acc/runtime/ubuntu:16.04"; fi

install:
	cp $(OUT_DIR)/$(TARGET) /usr/bin/$(TARGET)

