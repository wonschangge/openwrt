## kms-service

kms在嵌入式端侧的服务。

protobuf 从源码开始构建。

源码构建：

```sh
1981  git clone git@github.com:protocolbuffers/protobuf.git --branch=v3.17.3 --depth=1
1982  cd protobuf
1983  ./autogen.sh 
1984  ./configure 
1985  make -j16
1986  sudo make install
```

在宿主机测试：

CFLAGS="-I /usr/include/jsoncpp/" make
