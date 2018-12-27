all:test1 url_cache

test1:hash_table1.cc hash_table1_test.cc 
	g++ -fsanitize=address -g  $^ -o $@

url_cache:url_test.cc  container/fix_hash_table.cpp container/naive_hash_table.cpp container/url_cache.cpp
	g++ -I.  -O2  $^ -o $@  -lb2

clean:
	rm -rf test1 url_cache