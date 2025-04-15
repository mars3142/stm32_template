all: clean
	@make -C libopencm3 build -j 12
	@make -C bootloader all -j 12
	@make -C firmware all -j 12

debug: clean
	@make -C libopencm3 build -j 12
	@make -C bootloader all V=42
	@make -C firmware all V=42

cleanall: clean
	@make -C libopencm3 clean
	
clean:
	@make -C bootloader clean
	@make -C firmware clean

doc:
	@make -C libopencm3/doc clean doc

deploy: all
	@st-info --probe
	@st-flash write bootloader/bootloader.bin 0x08000000
	@st-flash --reset write firmware/firmware.bin 0x08008000
