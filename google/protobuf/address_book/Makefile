all:read write

addressbook.pb.cc:
	protoc --cpp_out=./ addressbook.proto

read:addressbook.pb.cc read.cpp
	g++ $^ -o $@ `pkg-config --cflags --libs protobuf`
write:addressbook.pb.cc write.cpp
	g++ $^ -o $@ `pkg-config --cflags --libs protobuf`

clean:
	rm -rf read write
	rm -rf *.pb.*
