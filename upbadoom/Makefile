CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -fstack-protector -Os -D_FORTIFY_SOURCE=2 -static

all: upbadoom

install : upbadoom
	install -m 0755 upbadoom /usr/sbin

upbadoom: upbadoom.cpp
	$(CXX) $(CXXFLAGS) -o upbadoom upbadoom.cpp

clean:
	rm -f upbadoom
