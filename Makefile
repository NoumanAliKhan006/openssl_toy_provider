# Default target
all: run

# Compiler and flags
CC = clang
CFLAGS = -Wall -Wextra -g -fPIC
LDFLAGS = -shared -Wl,--export-dynamic-symbol=OSSL_provider_init

# Source and target
SRCS = myprovider.c
OBJS = $(SRCS:.c=.o)
TARGET = myprovider.so

# Build the shared library
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Run the OpenSSL command (with suppressed output)
run: $(TARGET)
	@OPENSSL_MODULES=. openssl list -provider myprovider -verbose -providers

# Clean up the build
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets (not actual files)
.PHONY: all clean run
