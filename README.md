# FPGA-Docker

#### KAIST NCL 
#### Writer: Sung Hyun Kim, Eunju Yang

FPGA-Docker supports running container with accessibility to a target FPGA.
It manages status of all FPGAs in a server.
To start FPGA-Docker, write "device.fdocker" under ${FPGA-Docker-HOME}/out


### How to install

##### Dependencies

- docker
- protocol buffer (C++)


### How to run FPGA-Docker

` docker run --runtime=fpga-runtime -e ACC_VISIBLE_DEVICES=AlteraA10GX -e ACC_REQUIRE_LIBRARY=/usr/lib/libOpenCL.so `


