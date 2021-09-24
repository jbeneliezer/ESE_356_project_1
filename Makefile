src = $(wildcard *.cpp)
obj = $(src: .cpp=.o)

LDFLAGS =  -lsystemc -lm -g

CFLAGS = -I. -I$(SYSTEMC_HOME)/include -L. -L$(SYSTEMC_HOME)/lib-linux64 -Wl,-rpath=$(SYSTEMC_HOME)/lib-linux64

project1: $(obj)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm project1 
	rm *.vcd
