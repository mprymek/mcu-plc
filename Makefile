PLC_SRC_FILE ?= programs/blink.st

OPENPLC_DIR = OpenPLC_v3
SRC_GEN_DIR = plc-src

REAL_PLC_SRC_FILE = $(realpath $(PLC_SRC_FILE))
MATIEC_DIR = $(realpath $(OPENPLC_DIR)/utils/matiec_src)

.PHONY: all
all: generate

.PHONY: clean
clean:
	rm -rf $(SRC_GEN_DIR)

.PHONY: generate
generate: clean
	mkdir $(SRC_GEN_DIR)
	cd $(SRC_GEN_DIR) && $(MATIEC_DIR)/iec2c -I $(MATIEC_DIR)/lib $(REAL_PLC_SRC_FILE)

run:
	build/plc