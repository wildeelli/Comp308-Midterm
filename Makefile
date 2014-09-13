CC = g++
CFLAGS = -Wall -Wextra -pedantic
IPATH = -I/usr/X11/include -I/usr/pkg/include
LPATH = -L/usr/X11/lib -L/usr/pkg/lib
LDPATH = -Wl,-R/usr/pkg/lib 
RM = rm

BUILD = ./build/
DEBUG = ./debug/
SRC = ./src/

all: $(BUILD)MidTerm

$(BUILD)MidTerm: $(BUILD)quaternion.o $(BUILD)main.o $(BUILD)animation.o $(BUILD)G308_Geometry.o
	$(CC) -o $@ $^ -lm -lopengl32 -lGLU32 -lfreeglut $(LPATH) $(LDPATH)
	
$(BUILD)%.o:  $(SRC)%.cpp
	$(CC) $(CFLAGS) -c -O3 -o $@ $^ $(IPATH)
	
debug: $(DEBUG)MidTerm

$(DEBUG)MidTerm: $(DEBUG)quaternion.o $(DEBUG)main.o $(BUILD)animation.o $(BUILD)G308_Geometry.o
	$(CC) -o $@ $^ -lm -lopengl32 -lGLU32 -lfreeglut $(LPATH) $(LDPATH)
	
$(DEBUG)%.o:  $(SRC)%.cpp
	$(CC) -g $(CFLAGS) -c -o $@ $^ $(IPATH)
	
clean:
	$(RM) -f $(BUILD)*.o $(BUILD)*.gch $(BUILD)MidTerm*
	$(RM) -f $(DEBUG)*.o $(DEBUG)*.gch $(DEBUG)MidTerm*
