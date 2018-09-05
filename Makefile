.PHONY:all

OUT_DIR := out

RUNTIME_BIN := runc
HOOK_BIN := fpga-runtime-hook
MANAGER_BIN := fpga-manager

all: runtime hook manager

pre:
	@if [ ! -d "./$(OUT_DIR)" ]; then mkdir $(OUT_DIR); fi

runtime: pre
	make -C $(CURDIR)/FPGA-Runtime
	mv $(CURDIR)/FPGA-Runtime/$(OUT_DIR)/$(RUNTIME_BIN) $(OUT_DIR)/$(RUNTIME_BIN)

hook: pre
	make -C $(CURDIR)/FPGA-Runtime-Hook
	mv $(CURDIR)/FPGA-Runtime-Hook/$(OUT_DIR)/$(HOOK_BIN) $(OUT_DIR)/$(HOOK_BIN)

manager: pre
	make -C $(CURDIR)/FPGA-Manager
	mv $(CURDIR)/FPGA-Manager/$(OUT_DIR)/$(MANAGER_BIN) $(OUT_DIR)/$(MANAGER_BIN)

clean:
	make -C $(CURDIR)/FPGA-Runtime clean
	make -C $(CURDIR)/FPGA-Runtime-Hook clean
	make -C $(CURDIR)/FPGA-Manager clean
	rm -rf $(OUT_DIR)
