CXX = g++
EXE = proju

IMGUI_DIR = ../imgui/
BACKENDS_DIR = ../backends/
SERIALIB_DIR = ../serialib/

SOURCES = \
    main.cpp \
    powersupply.cpp \
    gpibdevice.cpp \
    gui.cpp \
    psugui.cpp

SOURCES += \
    $(IMGUI_DIR)/imgui.cpp \
    $(IMGUI_DIR)/imgui_draw.cpp \
    $(IMGUI_DIR)/imgui_tables.cpp \
    $(IMGUI_DIR)/imgui_widgets.cpp \
    $(IMGUI_DIR)/imgui_stdlib.cpp \
    $(BACKENDS_DIR)/imgui_impl_glfw.cpp \
    $(BACKENDS_DIR)/imgui_impl_opengl3.cpp \
    $(SERIALIB_DIR)/serialib.cpp

OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

CXXFLAGS = -std=c++23 -I$(IMGUI_DIR) -I$(BACKENDS_DIR) -I$(SERIALIB_DIR)
CXXFLAGS += -g -Wall -Wformat -Wextra -Werror

LIBS = -lGL -lX11 -lglfw -lrt -lm -ldl

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(BACKENDS_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(SERIALIB_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete: $(EXE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS)