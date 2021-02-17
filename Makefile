hiddenshuffle:
	$(CXX) -w -g -Isrc -O3 -std=c++11 -o hiddenshuffle hiddenshuffle.cpp	
.PHONY:
	clean
clean:
	rm -f -r bin/*.dSYM

