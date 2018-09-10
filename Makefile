.PHONY:all

OUT_DIR := out

RUNTIME_BIN := fpga-runtime
HOOK_BIN := fpga-runtime-hook
MANAGER_BIN := fpga-manager

all: $(RUNTIME_BIN) $(HOOK_BIN) $(MANAGER_BIN)

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
