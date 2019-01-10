all:proto client server

proto:heartbeat.pb.cc

heartbeat.pb.cc:
	protoc --cpp_out=./ heartbeat.proto

client:heartbeat.pb.cc client.cpp
	g++ $^ -o $@ `pkg-config --cflags --libs protobuf`

server:heartbeat.pb.cc server.cpp
	g++ $^ -o $@ `pkg-config --cflags --libs protobuf`

clean:
	rm -rf *.pb.*
	rm -rf client server
