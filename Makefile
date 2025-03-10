all: clean
	@make -C libopencm3 build -j 12
	@make -C bootloader all
	@make -C firmware all

cleanall: clean
	@make -C libopencm3 clean
	
clean:
	@make -C bootloader clean
	@make -C firmware clean

doc:
	@make -C libopencm3/doc clean doc
