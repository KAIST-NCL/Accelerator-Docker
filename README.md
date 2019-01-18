# Accelerator-Docker

Accelerator-Docker supports running container with accessibility to a target hardware accelerator (e.g., fpga or gpu).  
It can manage status of all accelerators in a server.
Accelerator-Docker supports automatic pass-through of PCIe-connected accelerators (FPGA or GPU) in container. Original docker requires setting for mounting device driver, libraries to access specific hardware in the container. In the Accelerator-Docker, however, it automatically runs a container following the pre-determined settings with simple option. Moreover, it provides common interface to manage heterogeneous accelerators. More details are described below.

<p align="center">
 <img src="images/Accelerator-Docker.png" alt="drawing" width="600" height="396"/>
</p>

## Getting Started
### Dependencies
- Docker
- Go >= 1.6
- Protocol Buffers (C++) & Protoc

For dependencies, refer to [Docker](https://docs.docker.com/install/linux/docker-ce/ubuntu/#set-up-the-repository), [Go](https://golang.org/dl/), [Protocol Buffers](https://github.com/protocolbuffers/protobuf/blob/master/src/README.md) 

### Installation
Install dependencies for build
```bash
$ sudo apt-get update && \
  sudo apt-get install -y build-essential git pkg-config libseccomp-dev libcap-dev libpci-dev libapparmor-dev libselinux1-dev
```
Now, clone this repository and make it.
```bash
$ git clone https://github.com/KAIST-NCL/Accelerator-Docker.git
$ cd Accelerator-Docker
$ make
$ sudo make install

$ sudo systemctl restart docker #Restart docker daemon to notify that new runtime is added
```

### Tested Environment
- Ubuntu 16.04
- Docker 18.06.1-ce
- Go 1.11
- Protocol Buffers 3.6.1

## How to run Accelerator-Docker
First, you need to provide device list you want to access from docker container as protocol buffer text form.  
**/etc/accelerator-docker/device.pbtxt**
```protobuf
accelerators:[{
    type: "xilinx.fpga/kcu1500",
    devices:{
        devices:[{
            name: "KCU-1500",
            device_file:[
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
    }
}]
```
- Files specified in 'library' section are mounted to '/usr/lib' of container, while the files specified in 'file' section are mounted to specific path.

```
$ docker run --runtime acc-runtime -e ACC_VISIBLE_DEVICES=KCU-1500
```

## Using ACC-Manager
Accelerator-Docker not only provides runtime hook for docker but also provides manager registered in device file.
Accelerator-Manager provides status for all devices and which container uses the hardware.

```
$ acc-manager list
```


## Authors
#### KAIST NCL
* [Sunghyun Kim](https://github.com/cqbqdd11519)
* [Eunju Yang](https://github.com/EunjuYang)

## License
This project is released under the [Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0), see [LICENSE](LICENSE) for more information.