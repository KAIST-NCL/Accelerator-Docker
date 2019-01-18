# ACC-Manager

ACC-Manager provides information of all registered accelerators.
acc manager list shows all accelerators and their status.
If an accelerator is used by a container, it print the container information using the hardware as "holder".

```
$ acc-manager list
```


```
+------------+--------------+------------------------+----------------+----------------+----------+
|     ID     |     Name     |          Type          |    PCI-Slot    |     Status     |  Holder  |
+------------+--------------+------------------------+----------------+----------------+----------+
| 234750     | QuadroM2000  | nvidia.com/gpu         | 0000:02:00.0   | Available      | 0        |
| 1121730    | KCU-1500     | xilinx.fpga/kcu-1500   | 0000:01:00.0   | Available      | 0        |
+------------+--------------+------------------------+----------------+----------------+----------+
```

