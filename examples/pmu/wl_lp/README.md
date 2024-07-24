# shell_os


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |

## Compile


- BL616/BL618

```
# 使用编译脚本genlp, 默认为 ldo 1.1v 模式, lpfw 不带 log:
./genlp
# 若要使用外部 dcdc 1.1v, 增加 --dcdc_1v1 参数, 如:
./genlp --dcdc_1v1
# 若要开启lpfw 的详细log, 用以debug, 增加 --lpfw_log 参数, 如:
./genlp --dcdc_1v1 --lpfw_log

# 注意, 如果修改了脚本参数，需要先执行 make clean 清除后再重新执行。
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```