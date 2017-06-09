all: mb_test

mb_test: bloom.cc test.cc
	$(CXX) -O2 -o $@ $^ 
	#$(CXX) -O0 -o $@ $^ 

clean:
	rm -rf mb_test *.o