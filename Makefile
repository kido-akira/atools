CXXFLAGS = -O3
LDFLAGS = -L. -latools -ltiff

OBJ = amisc.o atiff.o
ARC = libatools.a
EXEOBJ = main.o
EXE = a.out

all: $(EXE)

$(EXE): $(ARC) $(EXEOBJ)
	$(CXX) -g -o $@ $(EXEOBJ) $(LDFLAGS)

$(ARC): $(OBJ)
	$(AR) -rs $@ $(OBJ)

.cpp.o:
	$(CXX) -g -o $@ -c $< $(CXXFLAGS)

clean:
	rm -f *.o $(ARC) $(EXE)

depend:
	$(CXX) -MM $(OBJ:.o=.cpp) $(EXEOBJ:.o=.cpp) > Makefile.depend

-include Makefile.depend
