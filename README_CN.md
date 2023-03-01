# 操作指南

>此SDK仅适用于深圳乐动机器人有限公司销售的激光雷达产品，产品型号为:
> - LDROBOT LiDAR LD14

## 0. 获取雷达的Linux SDK
```bash
$ cd ~

$ mkdir  ldlidar_ws

$ cd ldlidar_ws

$ git clone  https://github.com/ldrobotSensorTeam/ldlidar_sl_sdk.git
```

## 1. 系统设置
- 第一步，通过板载串口或者USB转串口模块(例如,cp2102模块)的方式使雷达连接到你的系统主板.
- 第二步，设置雷达在系统中挂载的串口设备-x权限(以/dev/ttyUSB0为例)
	- 实际使用时，根据雷达在你的系统中的实际挂载情况来设置，可以使用`ls -l /dev`命令查看.

``` bash
$ cd ~/ldlidar_ws/ldlidar_sl_sdk

$ sudo chmod 777 /dev/ttyUSB0
```

## 2. 编译

```bash
$ cd ~/ldlidar_ws/ldlidar_sl_sdk
$ ./auto_build.sh
```

## 3. 运行
``` bash
$ ./build/ldlidar_sl_node <serial_number>
# 例如 ./build/ldlidar_sl_node /dev/ttyS0
```