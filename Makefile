CXX = g++
CC = gcc
CPPOBJS = src/main.o src/gs_xband.o network/network.o
COBJS = modem/src/libuio.o modem/src/libiio.o modem/src/adidma.o modem/src/txmodem.o adf4355/adf4355.o spibus/spibus.o gpiodev/gpiodev.o
CXXFLAGS = -I ./ -I ./include/ -I ./modem/ -I ./modem/include/ -I ./network/ -I ./adf4355/ -I ./spibus/ -Wall -pthread -std=c++17 -DGSNID=\"roofxband\"
TARGET = roof_xband.out
LFLAGS = -lpthread -liio

all: $(COBJS) $(CPPOBJS)
	$(CXX) $(CXXFLAGS) $(COBJS) $(CPPOBJS) -o $(TARGET) $(LFLAGS)
	sudo ./$(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

%.o: %.c
	$(CC) $(CXXFLAGS) -o $@ -c $<

.PHONY: clean

clean:
	$(RM) *.out
	$(RM) *.o
	$(RM) src/*.o
	$(RM) network/*.o
	$(RM) adf4355/*.o
	$(RM) gpiodev/*.o
	$(RM) spibus/*.o
	$(RM) modem/src/*.o