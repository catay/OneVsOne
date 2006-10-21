CXXFLAGS = -fPIC -frtti -I../../include -O
LDLIBS = -ldl
all : OneVsOne.so
OneVsOne.so : OneVsOne.o
		$(CXX) $(CXXFLAGS) -shared -rdynamic $^ -o $@
clean :
		-rm *.so
