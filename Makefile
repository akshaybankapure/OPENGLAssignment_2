# ------- Voxel Engine -------

OBJDIR = obj
OBJECT = $(addprefix $(OBJDIR)/,\
           opengl_window.o opengl_graphics_library.o opengl_event_handler.o opengl_shader.o opengl_renderer.o chunk.o main.o)

EXE = voxel_engine

CXX = g++
# CXXFLAGS = -ffast-math -g -DDEBUG
CXXFLAGS = -ffast-math -g -DDEBUG -DSHOW_FPS
# CXXFLAGS = -ffast-math -DSHOW_FPS
#CXXFLAGS = -O6 -ffast-math
LDFLAGS = -lGL -lGLEW -lglfw #-Lassimp -lfreetype

all: $(EXE)

$(EXE): $(OBJECT)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/opengl_window.o: src/window/opengl_window.cc | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/main.o: main.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/opengl_graphics_library.o: src/graphics_library/opengl_graphics_library.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/opengl_event_handler.o: src/event_handler/opengl_event_handler.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/opengl_shader.o: src/shader/opengl_shader.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/opengl_renderer.o: src/renderer/opengl_renderer.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/chunk.o: src/chunk/chunk.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)

distclean: clean
	rm -f $(EXE)
