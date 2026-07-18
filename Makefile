
SRCS := main.c hvf_utils.c regs.c
TARGET := hvf_gxf
ENTITLEMENTS := entitlements.plist

$(TARGET): $(SRCS) Makefile all_tests
	gcc $(SRCS) -o $(TARGET) -framework Hypervisor
	codesign -s - --entitlements $(ENTITLEMENTS) $(TARGET)

.PHONY: all_tests
all_tests:
	make -C tests

.PHONY: clean
clean:
	make clean -C tests
	rm -f hvf_gxf
