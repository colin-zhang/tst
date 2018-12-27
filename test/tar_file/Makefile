all: tar_tst
tar_tst:main.cpp tar_file.cpp gziphelper.cpp 
	g++ -o $@  $^ -lz
clean:
	rm -rf tar_tst *.o