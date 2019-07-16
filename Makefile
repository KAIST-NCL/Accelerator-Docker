.PHONY:all

OUT_DIR := out

RUNTIME_BIN := acc-runtime
RUNTIME_DIR := $(CURDIR)/ACC-Runtime
HOOK_BIN := acc-runtime-hook
HOOK_DIR := $(CURDIR)/ACC-Runtime-Hook
MANAGER_BIN := acc-manager
MANAGER_DIR := $(CURDIR)/ACC-Manager

all: $(RUNTIME_BIN) $(HOOK_BIN) $(MANAGER_BIN)

all-cont-ubuntu16.04: $(RUNTIME_BIN)-cont-ubuntu16.04 $(HOOK_BIN)-cont-ubuntu16.04 $(MANAGER_BIN)-cont-ubuntu16.04

pre:
	@if [ ! -d "./$(OUT_DIR)" ]; then mkdir $(OUT_DIR); fi

$(RUNTIME_BIN): pre
	make -C $(RUNTIME_DIR)
	cp $(RUNTIME_DIR)/$(OUT_DIR)/$@ $(OUT_DIR)/$@

$(HOOK_BIN): pre
	make -C $(HOOK_DIR)
	cp $(HOOK_DIR)/$(OUT_DIR)/$@ $(OUT_DIR)/$@

$(MANAGER_BIN): pre
	make -C $(MANAGER_DIR)
	cp $(MANAGER_DIR)/$(OUT_DIR)/$@ $(OUT_DIR)/$@

$(RUNTIME_BIN)-cont-ubuntu16.04: pre
	make -C $(RUNTIME_DIR) cont-ubuntu16.04
	cp $(RUNTIME_DIR)/$(OUT_DIR)/$(RUNTIME_BIN) $(OUT_DIR)/$(RUNTIME_BIN)

$(HOOK_BIN)-cont-ubuntu16.04: pre
	make -C $(HOOK_DIR) cont-ubuntu16.04
	cp $(HOOK_DIR)/$(OUT_DIR)/$(HOOK_BIN) $(OUT_DIR)/$(HOOK_BIN)

$(MANAGER_BIN)-cont-ubuntu16.04: pre
	make -C $(MANAGER_DIR) cont-ubuntu16.04
	cp $(MANAGER_DIR)/$(OUT_DIR)/$(MANAGER_BIN) $(OUT_DIR)/$(MANAGER_BIN)
	cp $(MANAGER_DIR)/$(OUT_DIR)/libprotobuf* $(OUT_DIR)/

install:
	make -C $(RUNTIME_DIR) install
	make -C $(HOOK_DIR) install
	make -C $(MANAGER_DIR) install
	if [ -f "/etc/docker/daemon.json" ]; then mv /etc/docker/daemon.json /etc/docker/daemon.json.bak; fi
	\cp ./daemon.json /etc/docker/daemon.json
uninstall:
	rm -f /usr/bin/$(RUNTIME_BIN) /usr/bin/$(HOOK_BIN) /usr/bin/$(MANAGER_BIN)

clean:
	make -C $(RUNTIME_DIR) clean
	make -C $(HOOK_DIR) clean
	make -C $(MANAGER_DIR) clean
	rm -rf $(OUT_DIR)
