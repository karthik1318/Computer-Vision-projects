
# Define executable name
BIN = omr

# Define source files
SRCS = ./ImageResize.cpp ./Gaussblur.cpp ./TemplateDetector.cpp ./DrawText.cpp ./Canny.cpp ./a1.cpp ./utils.cpp ./Hough.cpp ./detection_utils.cpp ./EdgeTemplateDetector.cpp ./A1Debug.cpp ./Sobelblur.cpp ./Config.cpp ./Kernel.cpp ./SImage.cpp ./StaffDetector.cpp

# Define header file paths
INCPATH = -I./

# Define the -L library path(s)
LDFLAGS = 

# Define the -l library name(s)
LIBS = -lpng

# Define Compile Flags
COMPILEFLAGS = -Wall -pedantic -Wno-unused-variable -Wno-unused-but-set-variable -g

# Define Link Flags
LINKFLAGS = -g

# Do not make any changes below!
OBJS      = $(SRCS:.cpp=.o)
CXX       = g++
DEP_FILE  = .depend

.PHONY = all clean distclean


# Main entry point
#
all: depend $(BIN)


# For linking object file(s) to produce the executable
#
$(BIN): $(OBJS)
	@echo Linking $@
	@$(CXX) $(LINKFLAGS) $^ $(LDFLAGS) $(LIBS) -o $@


# For compiling source file(s)
#
.cpp.o:
	@echo Compiling $<
	@$(CXX) -c $(COMPILEFLAGS) $(INCPATH) $< -o $(<:.cpp=.o)


# For cleaning up the project
#
clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) $(BIN)
	$(RM) $(DEP_FILE)


# For determining source file dependencies
#
depend: $(DEP_FILE)
	@echo "Dependencies generated."
	
$(DEP_FILE):
	@echo Generating dependencies in $@
	@-$(CXX) -E -MM $(COMPILEFLAGS) $(INCPATH) $(SRCS) >> $(DEP_FILE)

ifeq (,$(findstring clean,$(MAKECMDGOALS)))
ifeq (,$(findstring distclean,$(MAKECMDGOALS)))
-include $(DEP_FILE)
endif
endif

