# Build system for Wordup-Fe

# Settings

## Project

PROJNAME :=		wordup-fe
ifeq ($(OS), Windows_NT)
OBJNAME :=		$(PROJNAME).exe
else
OBJNAME :=		$(PROJNAME).bin
endif
SRC :=			$(wildcard src/*.cpp)
OBJS :=			$(subst src/,obj/,$(subst .cpp,.o,$(SRC)))
HFILES :=		$(wildcard include/*.hpp)

## Compiler

CPPC :=			g++
CPPFLAGS :=		-std=c++17 -O2 -Wall -Werror -Iinclude -Inatevolve-ark/include
LD :=			g++
LDFLAGS :=		-Lnatevolve-ark -lnatevolve

# Targets

## Helper

.PHONY: all
all: $(OBJNAME)

.PHONY: clean
clean:
	$(MAKE) -C natevolve-ark clean
	rm -rf obj/
	rm -rf $(OBJNAME)

## Main

libnatevolve.a:
	git submodule update --init --recursive
	$(MAKE) -C natevolve-ark

obj/%.o: src/%.cpp $(HFILES)
	mkdir -p obj
	$(CPPC) -o $@ $(CPPFLAGS) -c $<

$(OBJNAME): $(OBJS) libnatevolve.a
	$(LD) -o $@ $(OBJS) $(LDFLAGS)

