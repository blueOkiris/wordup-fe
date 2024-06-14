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
				$(wildcard imgui-sfml/*.cpp) \
				$(wildcard ImGui-Addons/FileBrowser/*.cpp)
OBJS :=			$(foreach file,$(SRC),obj/$(notdir $(basename $(file))).o)
HFILES :=		$(wildcard include/*.hpp)

## Compiler

CPPC :=			g++
ifeq ($(OS), Windows_NT)
CPPFLAGS :=		-std=c++17 -O2 -Wall -Werror \
				-Iinclude -Inatevolve-ark/include -Iimgui -Iimgui-sfml -IImGui-Addons/FileBrowser \
				-ISFML/include
else
CPPFLAGS :=		-std=c++17 -O2 -Wall -Werror \
				-Iinclude -Inatevolve-ark/include -Iimgui -Iimgui-sfml -IImGui-Addons/FileBrowser \
				$(shell pkg-config --cflags sfml-all)
endif
LD :=			g++
ifeq ($(OS), Windows_NT)
LDFLAGS :=		-mwindows -Lnatevolve-ark -LSFML/lib -lnatevolve -lopengl32 \
				-lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
else
LDFLAGS :=		-Lnatevolve-ark -lnatevolve -lGL \
				$(shell pkg-config --libs sfml-all)
endif

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

natevolve-ark/libnatevolve.a:
	$(MAKE) -C natevolve-ark

obj/%.o: src/%.cpp $(HFILES)
ifeq ($(OS), Windows_NT)
	-mkdir obj
else
	mkdir -p obj
endif
	$(CPPC) -o $@ $(CPPFLAGS) -c $<

obj/%.o: imgui/%.cpp $(HFILES)
ifeq ($(OS), Windows_NT)
	-mkdir obj
else
	mkdir -p obj
endif
	$(CPPC) -o $@ $(CPPFLAGS) -c $<

obj/%.o: imgui-sfml/%.cpp $(HFILES)
ifeq ($(OS), Windows_NT)
	-mkdir obj
else
	mkdir -p obj
endif
	$(CPPC) -o $@ $(CPPFLAGS) -c $<

obj/%.o: ImGui-Addons/FileBrowser/%.cpp $(HFILES)
ifeq ($(OS), Windows_NT)
	-mkdir obj
else
	mkdir -p obj
endif
	$(CPPC) -o $@ $(CPPFLAGS) -c $<

$(OBJNAME): $(OBJS) natevolve-ark/libnatevolve.a
	$(LD) -o $@ $(OBJS) $(LDFLAGS)

