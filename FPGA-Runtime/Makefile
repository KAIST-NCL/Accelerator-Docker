.PHONY:all

TMP_GO_PATH := .tmp_go_path
OUT_DIR := ./out

TMP_GO_ABS := $(CURDIR)/$(TMP_GO_PATH)
ENV_GOPATH := env GOPATH=$(TMP_GO_ABS)

RUNC_REPO:=github.com/opencontainers/runc

TARGET := fpga-runtime

all: $(clean) $(TARGET)

$(TARGET): 
	@if [ ! -d "$(TMP_GO_ABS)" ]; then mkdir $(TMP_GO_ABS); fi
	@echo Setting temp. GOPATH as [$(TMP_GO_ABS)]
	$(ENV_GOPATH) go get -d $(RUNC_REPO)/...
	patch -d$(TMP_GO_ABS)/src/$(RUNC_REPO) -p1 < fpga-runtime.patch
	$(ENV_GOPATH) make -C $(TMP_GO_ABS)/src/$(RUNC_REPO)/ BUILDTAGS='apparmor seccomp'
	@if [ ! -d "$(OUT_DIR)" ]; then mkdir $(OUT_DIR); fi
	@mv $(TMP_GO_ABS)/src/$(RUNC_REPO)/$@ $(OUT_DIR)/$@

clean: 
	rm -rf $(TMP_GO_PATH)
	rm -rf $(OUT_DIR)
