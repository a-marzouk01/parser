CC = clang
CFLAGES = -lstdc++ -Wall -Wextra -O2 

target = parser

all: $(target)

$(target):
	$(CC) main.cpp $(CFLAGES) -o $(target)

clean:
	rm -f $(target)

rebuild: clean all
