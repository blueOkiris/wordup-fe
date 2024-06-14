# Build system for Wordup-Fe

# Settings

## Project

PROJNAME :=		wordup-fe
ifeq ($(OS), Windows_NT)
OBJNAME :=		$(PROJNAME).exe
else
OBJNAME :=		$(PROJNAME).bin
endif
SRC :=			$(wildcard src/*.cpp) \
				$(wildcard imgui/*.cpp) \
				$(wildcard imgui-sfml/*.cpp)
OBJS :=			$(foreach file,$(SRC),obj/$(notdir $(basename $(file))).o)
HFILES :=		$(wildcard include/*.hpp)

## Compiler

CPPC :=			g++
CPPFLAGS :=		-std=c++17 -O2 -Wall -Werror \
				-Iinclude -Inatevolve-ark/include -Iimgui -Iimgui-sfml
LD :=			g++
LDFLAGS :=		-Lnatevolve-ark -lnatevolve $(shell pkg-config --libs sfml-all) -lGL

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
	$(MAKE) -C natevolve-ark

obj/%.o: src/%.cpp $(HFILES)
	mkdir -p obj
	$(CPPC) -o $@ $(CPPFLAGS) -c $<

obj/%.o: imgui/%.cpp $(HFILES)
	mkdir -p obj
	$(CPPC) -o $@ $(CPPFLAGS) -c $<

obj/%.o: imgui-sfml/%.cpp $(HFILES)
	mkdir -p obj
	$(CPPC) -o $@ $(CPPFLAGS) -c $<

$(OBJNAME): $(OBJS) libnatevolve.a
	$(LD) -o $@ $(OBJS) $(LDFLAGS)

