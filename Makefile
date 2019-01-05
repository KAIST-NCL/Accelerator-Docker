.PHONY:all

OUT_DIR := out

RUNTIME_BIN := fpga-runtime
HOOK_BIN := fpga-runtime-hook
MANAGER_BIN := fpga-manager

all: $(RUNTIME_BIN) $(HOOK_BIN) $(MANAGER_BIN)

all-cont-ubuntu16.04: $(RUNTIME_BIN)-cont-ubuntu16.04 $(HOOK_BIN)-cont-ubuntu16.04 $(MANAGER_BIN)-cont-ubuntu16.04

pre:
	@if [ ! -d "./$(OUT_DIR)" ]; then mkdir $(OUT_DIR); fi

$(RUNTIME_BIN): pre
	make -C $(CURDIR)/FPGA-Runtime
	cp $(CURDIR)/FPGA-Runtime/$(OUT_DIR)/$@ $(OUT_DIR)/$@

$(HOOK_BIN): pre
	make -C $(CURDIR)/FPGA-Runtime-Hook
	cp $(CURDIR)/FPGA-Runtime-Hook/$(OUT_DIR)/$@ $(OUT_DIR)/$@

$(MANAGER_BIN): pre
	make -C $(CURDIR)/FPGA-Manager
	cp $(CURDIR)/FPGA-Manager/$(OUT_DIR)/$@ $(OUT_DIR)/$@

$(RUNTIME_BIN)-cont-ubuntu16.04: pre
	make -C $(CURDIR)/FPGA-Runtime cont-ubuntu16.04
	cp $(CURDIR)/FPGA-Runtime/$(OUT_DIR)/$(RUNTIME_BIN) $(OUT_DIR)/$(RUNTIME_BIN)

$(HOOK_BIN)-cont-ubuntu16.04: pre
	make -C $(CURDIR)/FPGA-Runtime-Hook cont-ubuntu16.04
	cp $(CURDIR)/FPGA-Runtime-Hook/$(OUT_DIR)/$(HOOK_BIN) $(OUT_DIR)/$(HOOK_BIN)

$(MANAGER_BIN)-cont-ubuntu16.04: pre
	make -C $(CURDIR)/FPGA-Manager cont-ubuntu16.04
	cp $(CURDIR)/FPGA-Manager/$(OUT_DIR)/$(MANAGER_BIN) $(OUT_DIR)/$(MANAGER_BIN)
	cp $(CURDIR)/FPGA-Manager/$(OUT_DIR)/libprotobuf* $(OUT_DIR)/

install:
	\cp $(CURDIR)/$(OUT_DIR)/$(RUNTIME_BIN) /usr/bin/$(RUNTIME_BIN)
	\cp $(CURDIR)/$(OUT_DIR)/$(HOOK_BIN) /usr/bin/$(HOOK_BIN)
	\cp $(CURDIR)/$(OUT_DIR)/$(MANAGER_BIN) /usr/bin/$(MANAGER_BIN)
	if [ -f "/etc/docker/daemon.json" ]; then mv /etc/docker/daemon.json /etc/docker/daemon.json.bak; fi
	\cp ./daemon.json /etc/docker/daemon.json
uninstall:
	rm -f /usr/bin/$(RUNTIME_BIN) /usr/bin/$(HOOK_BIN) /usr/bin/$(MANAGER_BIN)

clean:
	make -C $(CURDIR)/FPGA-Runtime clean
	make -C $(CURDIR)/FPGA-Runtime-Hook clean
	make -C $(CURDIR)/FPGA-Manager clean
	rm -rf $(OUT_DIR)
