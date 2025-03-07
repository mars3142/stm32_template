all:
	@make -C libopencm3 clean build -j 12
	@make -C bootloader clean images
	@make -C firmware clean images

doc:
	@make -C libopencm3/doc clean doc
