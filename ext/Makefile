INSTALL_DIR := $(CURDIR)/install
NOISE_C_DIR := noise-c

.PHONY: always libs noise-c

libs: noise-c

noise-c: always $(INSTALL_DIR)/lib/libnoisekeys.a
$(INSTALL_DIR)/lib/libnoisekeys.a:
	cd $(NOISE_C_DIR);\
	./autogen.sh;\
	mkdir -p build;\
	cd build;\
	../configure --prefix=$(INSTALL_DIR);\
	$(MAKE);\
	$(MAKE) install

always:
	mkdir -p $(INSTALL_DIR)

clean:
	rm -r $(INSTALL_DIR)
	$(MAKE) -C $(NOISE_C_DIR)/build clean
	rm -r $(NOISE_C_DIR)/build
