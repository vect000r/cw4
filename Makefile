.SUFFIXES: .c .h .x .o

CFLAGS = -pedantic -Wall -std=c99
LFLAGS = -pedantic -Wall -std=c99
CO = gcc
LD = $(CO)

NAME = cw4

OBJS = $(NAME).o

EXEC = $(NAME).x




.PHONY: help
help:
	@echo "make all		Kompiluje wszystkie programy"
	@echo "make run-all	Uruchamia wszystkie programy"
	@echo "make run-$(NAME)    Uruchamia program $(NAME)"
	@echo "make clean	Usuwa pliki *.o oraz *.x"
.PHONY: all
all: $(EXEC) 

$(EXEC): $(OBJS)
	$(LD) $(LFLAGS) -o $@ $^

%.o: %.c %.h
	$(CO) $(CFLAGS) -c $<

.PHONY: run-$(EXEC) 

run-$(NAME): $(EXEC)
	./$(EXEC)

.PHONY: clean

clean:
	rm *.o *.x

.PHONY: run-all
run-all: $(EXEC) 
	./$(EXEC)
	
	

