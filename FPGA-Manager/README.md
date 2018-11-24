# FPGA-Manager

FPGA-Manager provides information of all registered FPGAs.
fpga manager list shows all accelerators and their status.
If a fpga is used by a container, it print the container information using the hardware as "holder".

```
$ fpga-manager list
```


```
+------+--------------------+----------+----------------+----------------+----------+
|  ID  |        Name        |   Type   |    PCI-Slot    |     Status     |  Holder  |
+------+--------------------+----------+----------------+----------------+----------+
| 0    | KCU_1500           | FPGA     | 0000:01:00.0   | Available      | 0        |
| 1    | QuadroM2000        | GPU      | 0000:02:00.0   | Available      | 0        |
+------+--------------------+----------+----------------+----------------+----------+
```

