# FPGA-Docker

FPGA-Docker supports running container with accessibility to a target FPGA.  
It manages status of all FPGAs in a server.

## Getting Started
#### Dependencies
- Docker
- Go >= 1.6
- Protocol Buffers (C++) & Protoc

For dependencies, refer to [Docker](https://docs.docker.com/install/linux/docker-ce/ubuntu/#set-up-the-repository), [Go](https://golang.org/dl/), [Protocol Buffers](https://github.com/protocolbuffers/protobuf/blob/master/src/README.md) 

#### Installing
Install dependencies for build
```bash
$ sudo apt-get update && \
  sudo apt-get install -y build-essential git pkg-config libseccomp-dev libcap-dev libpci-dev libapparmor-dev libselinux1-dev
```
Now, clone this repository and make it.
```bash
$ git clone https://github.com/EunjuYang/FPGA-Docker.git
$ cd FPGA-Docker
$ make
$ sudo make install
```

#### Tested Environment
- Ubuntu 16.04
- Docker 18.06.1-ce
- Go 1.11
- Protocol Buffers 3.6.1

## How to run FPGA-Docker
First, you need to provide device list you want to access from docker container as protocol buffer text form.  
**/etc/fpga-docker/device.pbtxt**
```
devices :[{
    name: "Xilinx_1",
    type:"FPGA",
    pci: "0000:01:00.0",
    device_driver:[
        "/dev/xcldev1",
        "/dev/xdmadev1"
    ],
    library:[
        "/PATH/TO/LIB/libxilinxopencl.so"
    ],
    file:[{
        src: "/PATH/IN/HOST/libxclgemdrv.so",
        dst: "/PATH/IN/CONTAINER/libxclgemdrv.so",
    }],
    env:[{
        key: "XILINX_SDX",
        val: "/SDX"
    }]
}]
```
(Files specified in 'library' section are mounted to '/usr/lib' of container, while the files specified in 'file' section are mounted to specific path.)

```
$ docker run --runtime fpga-runtime -e ACC_VISIBLE_DEVICES=Xilinx_1
```


## Authors
#### KAIST NCL
* Sunghyun Kim
* Eunju Yang

## License
