VSRC = ./vsrc/$(TARGET).v
CSRC = ./csrc/$(TARGET).cpp
WVFM = ./waveform/$(TARGET).fst

all:
	verilator --cc --exe --build -j 0 -Wall --trace-fst $(VSRC) $(CSRC)

sim: all
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	./obj_dir/V$(TARGET)
	gtkwave $(WVFM)

include ../Makefile
