CXXFLAGS = -fPIC -frtti -I../../include -O
LDLIBS = -ldl
all : OneVsOne.so
OneVsOne.so : OneVsOne.o UrlHandler.o INIParser.o
		$(CXX) $(CXXFLAGS) -shared -rdynamic $^ -o $@
remote :
	make clean && scp * kaiya:work/bzflag/plugins/OneVsOne && ssh kaiya "cd work/bzflag/plugins/OneVsOne &&  make clean && make"
clean :
		-rm -f *.so *.o
