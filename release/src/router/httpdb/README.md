# Httpdb

Standalong version for http server using in next softcenter.  外挂型软件中心，完全不改原有固件的任何代码。 通过反向代理把原先的 http server 替换。

# Feature

1. json api for [skipdbv2](https://github.com/koolshare/skipdbv2)
2. static files support
3. shell calling support
4. reverse proxy for existing httpd server
5. auth with exists httpd server
6. http/https support
7. single thread, event driving, low memory used.

# Build

```
make build
cd build
cmake28 ..
make

./bin/httpdb -p 8000 -s 8443 -r 10.1.1.1:80 -c ../tests/ssl.pem -w ../tests/web_root
```

# Param information

* -p 8000 本地http 端口
* -s 8443 本地运行的 https 端口
* -r 10.1.1.1:80 反向代理路由器的 web 界面
* -c ../tests/ssl.pem https 的证书
* -w ../tests/web_root 本地的 web 目录, 若目录地址与路由器 web 界面一致，则自动替换 web 地址

# Interface

* curl -X POST -d '{"id":3434343, "method":"config.sh", "params":[33,44],"fields":{"hello":"cc","hd":"oooo","hf":"nnn"}}' http://127.0.0.1:8000/_api/
  * 向服务器存储一个对象
  * 请求 id 为 3434343
  * 运行的脚本为 config.sh
  * 脚本参数为 33,44 智能为数字，实际脚本运行参数为：config.sh 3434343 33 44
  * 向 dbus 存储的对象是 "hello":"cc","hd":"oooo","hf":"nnn"，路由器通过命令 dbus list h，会把参数拿到
  * 直接返回脚本执行的结果。（如果脚本 sleep ，也将自动等待 sleep 结束才返回）

* curl -X GET http://127.0.0.1/_api/he,hd
  * 拿到 dbus 中前缀为 he 或 为 hd 的所有参数

* curl -X POST -d 'test' http://127.0.0.1:8000/_resp/3434343
  * 设置返回值。当脚本运行结束之后，运行此参数，可以临时设置脚本的返回值为 test。保存 1 分钟超时

* curl -X GET http://127.0.0.1:8000/_result/3434343
  * 脚本运行结束之后，运行此接口拿到返回值
  * 提交之后可以等待脚本结束，也可以论训脚本得到请求的结果。此接口为论训接口。

* http://127.0.0.1:8000/_root/xxx
  * 拿到 web_root 下面的所有文件

* 替换路由器 web 目录现有文件
  * 首先 路由器地址为 http://10.1.1.1:80，有一个文件是 http://10.1.1.1/jquery.js
  * 如果路由器已有的 jquery.js 并不是咱们想要的版本, 则在 root 目录放一个咱们版本的 jquery.js，则 /jquery.js 则是被替换的版本

