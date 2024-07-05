CC = gcc
CFLAGS += -Wall

all: kms-protobuf
	$(CC) $(CFLAGS) \
		main.c  rest.c mqtt.c cJSON.c http.c util.c log.c data_parse.cpp \
		KmsCore.pb.cc \
		-I. \
		-ljsoncpp \
		-lprotobuf -lstdc++ \
		-lcurl -lssl -lcrypto -lmosquitto \
		-DLOG_USE_COLOR \
		-g \
		-o kms-service

kms-protobuf:
	protoc -I $(STAGING_DIR_HOST)/lib/ -I ./ --cpp_out=. KmsCore.proto
