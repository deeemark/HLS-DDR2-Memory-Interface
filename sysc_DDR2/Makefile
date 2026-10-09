CXX      = g++
SYSTEMC  = /proj/cad/cwb-6.1/osci
TARGET   = ddr2_top_test

CXXFLAGS = -std=gnu++98 -D_GLIBCXX_USE_CXX11_ABI=0 -O1 -I$(SYSTEMC)/include -I.
LDFLAGS  = -L$(SYSTEMC)/lib-linux64 -lsystemc -lm

SRCS = main.cpp tb_ddr2_top.cpp ddr2_controller.cpp \
       ddr2_write_phy_pos.cpp ddr2_write_phy_neg.cpp ddr2_write_phy.cpp \
       ddr2_read_phy_pos.cpp ddr2_read_phy_neg.cpp ddr2_read_phy.cpp \
       ddr2_top.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all run wave clean

all: $(TARGET)
	LD_LIBRARY_PATH=$(SYSTEMC)/lib-linux64:$$LD_LIBRARY_PATH ./$(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all

# S2CBench-style waveform build: rebuild with WAVE_DUMP and produce trace_behav.vcd.
wave: clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -DWAVE_DUMP" all

clean:
	rm -f $(TARGET) $(OBJS) trace_behav.vcd
