.PHONY:all

INC_DIR = include
SRC_DIR = src

FLAGS = -I${INC_DIR} -lpci

all:
	g++ ${SRC_DIR}/*.cpp -D_GNU_SOURCE ${FLAGS} -o acc-manager -std=c++11

install:
	cp acc-manager /usr/bin/acc-manager
