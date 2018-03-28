CC=g++

all:
	$(CC) -Wall -Werror -o residue-logger main.cc -DELPP_THREAD_SAFE -D_GLIBCXX_USE_CXX11_ABI=0 -std=c++11 -lresidue -lpthread
	strip residue-logger
