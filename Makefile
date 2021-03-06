# Grupo: 2
# Ana Caroline Fernandes Spengler 8532356
# Paulo Bardes                    8531932
# Renato Goto                     8516692

EXEC = T2

WARNINGS = -pedantic -Wall -Wextra -Wcast-align -Wcast-qual \
       -Wdisabled-optimization -Wformat=2 -Wlogical-op -Wmissing-declarations \
	   -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wsign-conversion \
	   -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused

CFLAGS = $(WARNINGS) -std=gnu11 -g

CPPFLAGS = -D DEBUG

LDFLAGS = -lcurses

SRCS = $(shell find -name '*.c')
HDRS = $(shell find -name '*.h')
RES  = ./tests ./data

OBJS = $(SRCS:=.o)
DEPS = $(SRCS:=.dep)

.PHONY: clean all run zip

all: $(EXEC)

#Linka e gera o executavel
$(EXEC): $(DEPS) $(OBJS)
	$(CC) $(CFALGS) -o $(EXEC) $(OBJS) $(LDFLAGS)

#Compila cada arquivo
%.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

#Gera as dependencias
%.dep: %
	$(CC) -MM -MT "$@ $(@:.dep=.o)" $(CPPFLAGS) $< -MF $@

clean:
	rm -f $(OBJS) $(DEPS) $(EXEC);

zip:
	zip -r $(EXEC).zip $(SRCS) $(HDRS) Makefile 

-include $(DEPS)
