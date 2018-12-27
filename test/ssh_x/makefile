all:ssh_test

ssh_test:sftp_client.cpp sftp_test.cpp
	$(CXX) $^ $(CCFLAGS) -lssh2 -lpthread -o $@ 