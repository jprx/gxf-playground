
SRCS := hvf_gxf.c regs.c
TARGET := hvf_gxf
ENTITLEMENTS := entitlements.plist

$(TARGET): $(SRCS) Makefile experiment/kernel.bin
	gcc $(SRCS) -o $(TARGET) -framework Hypervisor
	codesign -s - --entitlements $(ENTITLEMENTS) $(TARGET)

.PHONY: experiment/kernel.bin
experiment/kernel.bin:
	make -C experiment

.PHONY: clean
clean:
	rm -f $(TARGET)
