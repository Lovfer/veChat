# veChat
A simple multi-person chat room program...



## 一、veClient



### 1. 默认参数配置

``` sh
$ 服务器IP: 127.0.0.1(默认值)
$ 通信端口: 8888(默认值)
```


### 2. 自定义参数

``` sh
$ 用户名:   user_name 
$ 服务端IP: ip_address 
$ 通信端口: port
```


### 3. 运行示例

``` sh
# 根据需要指定参数

# 示例1：自定义用户名字段参数,IP与Port字段使用默认参数
$ ./veClient 刘备

# 示例2：自定义用户名与IP字段参数,Port字段使用默认参数
$ ./veClient 刘备 192.168.1.103

# 示例3：自定义所有字段参数
$ ./veClient 刘备 192.168.1.103 8801
```


### 4. 后记备注

``` sh
$ 1.veClient客户端必须指定 [user_name] 字段,标识用户信息
```



## 二、veServer



### 1. 默认参数配置

``` sh
$ 监听IP: INADDR_ANY(默认值,监听本机所有IP)
$ 监听端口: 8888(默认值)
```


### 2. 自定义参数

``` sh 
$ 监听IP: ip_address 
$ 监听端口: port
```


### 3. 运行示例

``` sh
# 根据需要指定参数

# 示例1：所有字段使用默认参数
$ ./veClient

# 示例2：自定义IP字段参数,Port字段使用默认参数
$ ./veClient 192.168.1.103

# 示例3：自定义所有字段参数
$ ./veClient 192.168.1.103 8801
```


### 4. 后记备注

``` sh
$ 1.veServer服务端的所有字段可以使用默认参数
```