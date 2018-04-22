OBJS = rmptest.o 
TARGET = rmptest
LDFLAGS = -lbcm2835 -lpigpio -lrt
CFLAGS = -std=c99

.SUFFIXES: .c .o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -std=c99 -o $@ -g $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)