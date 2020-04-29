TARGET	= msh
SRC1	= msh.c
CFLAGS	= -Wall

$(TARGET): $(SRC1)
	gcc $(CFLAGS) $(SRC1) -o $(TARGET)

clean:
	rm msh
