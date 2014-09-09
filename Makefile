CC = g++
CFLAGS = -g -Wall -Wextra -pedantic 
IPATH = -I/usr/X11/include -I/usr/pkg/include
LPATH = -L/usr/X11/lib -L/usr/pkg/lib
LDPATH = -Wl,-R/usr/pkg/lib 
RM = rm

BUILD = ./build/
SRC = ./src/

all: $(BUILD)MidTerm

$(BUILD)MidTerm: $(BUILD)quaternion.o $(BUILD)main.o #$(BUILD)animation.o
	$(CC) -o $@ $^ -lm -lopengl32 -lGLU32 -lfreeglut $(LPATH) $(LDPATH)
	
$(BUILD)%.o:  $(SRC)%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ $(IPATH)
	
clean:
	$(RM) -f $(BUILD)*.o $(BUILD)*.gch $(BUILD)Ass2*
