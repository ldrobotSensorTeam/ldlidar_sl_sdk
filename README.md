# Instructions
> This SDK is only applicable to the LiDAR products sold by Shenzhen LDROBOT Co., LTD. The product models are :
> - LDROBOT LiDAR LD14
> - LDROBOT LiDAR LD14P

## step 0. get LiDAR Linux SDK
```bash
$ cd ~

$ mkdir  ldlidar_ws

$ cd ldlidar_ws

$ git clone  https://github.com/ldrobotSensorTeam/ldlidar_sl_sdk.git
```
## step 1: system setup
- Connect the LiDAR to your system motherboard via an onboard serial port or usB-to-serial module (for example, CP2102 module).

- Set the -x permission for the serial port device mounted by the radar in the system (for example, /dev/ttyUSB0)

  - In actual use, the LiDAR can be set according to the actual mounted status of your system, you can use 'ls -l /dev' command to view.

``` bash
$ cd ~/ldlidar_ws/ldlidar_sl_sdk

$ sudo chmod 777 /dev/ttyUSB0
```

## step 2: build

``` bash
$ cd ~/ldlidar_ws/ldlidar_sl_sdk
$ ./auto_build.sh
```

## step 3: run
``` bash
$ ./build/ldlidar_sl_node <product_type> <serial_number>
# eg:
# LDLiDAR LD14
$ ./build/ldlidar_sl_node LD14  /dev/ttyUSB0
# LDLiDAR LD14P, measuring frequnecy is 2300Hz
$ ./build/ldlidar_sl_node LD14P_2300HZ  /dev/ttyUSB0
# LDLiDAR LD14P, measuring frequnecy is 4000Hz
$ ./build/ldlidar_sl_node LD14P_4000HZ  /dev/ttyUSB0
```
