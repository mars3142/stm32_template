all:
	@make -C libopencm3 clean build
	@make -C bootloader clean all
	@make -C firmware clean all

doc:
	@make -C libopencm3/doc clean doc
