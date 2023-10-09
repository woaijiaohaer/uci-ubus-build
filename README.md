# 在ubuntu上编译libubox、ubus、uci等源码。

```bash
git clone https://github.com/woaijiaohaer/uci-ubus-build.git
```

先克隆源码。

> 先编译json-c

```bash
tar -vxzf json-c-0.12.tar.gz
```

解压之后

```bash
cd json-c-0.12/

#编译安装json-c库
sed -i s/-Werror// Makefile.in   

./configure --prefix=/usr/local --disable-static

make -j4

make install

ln -sf /usr/local/include/json-c /usr/local/include/json

ldconfig
```

**编译libubox 、uci 、ubus**

```bash
tar -xvzf libubox-2014-08-04.tar.gz
```

解压之后

```bash
cd libubox-2014-08-04/

mkdir build

cd build

cmake -DBUILD_LUA=off ..

make -j4

make install

ldconfig
```

剩下的uci、ubus是一样的操作。

