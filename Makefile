# the compiler: gcc for C program, define as g++ for C++
cc = g++

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CFLAGS  = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network

# The build target
TARGET = chess

make: *.cpp
	$(cc) -c *.cpp 
	$(cc) -o $(TARGET) *.o $(CFLAGS) 

run: make
	./chess

clean:
	del *.o $(TARGET).exe