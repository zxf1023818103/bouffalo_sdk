# OpenThread Border Router Over Ethernet Infrastructure network

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```


## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

### Hardwares

  Prepare:
    - One board with Ethernet Interface, named OTBR, which flashes this example.
      - Please configure `CONFIG_ETHERNET_PHY` in proj.conf to specific ethernet PHY driver for your board.
    - One board, named OTCLI, which flashes openthread command line example.

### Steps

  - Connect OTBR and your test PC station in to same router with Ethernet cable.
  - Open OTBR UART, power up or reset the board.
    > OTBR automatically starts TCP/IP stack to require IP address after power up or reset; 
    > then starts/attaches Thread network, and then plays as role for Thread packets Routing and forwarding.

  - Type following command in OTBR UART to get Thread network credential

    ```shell
    bouffalolab />otc dataset active -x
    dataset active -x
    0e080000000000010000000300001735060004001fffe002086e0f33c1aa1076d30708fd563c8a2072356f05101f5be8fb42b487278a27619c7a0f2470030f4f70656e5468726561642d3765363501027e6504107ccd496e4c41be0651547fe30355ff330c0402a0f7f8
    Done
    ```
    > Copy hex string above for next step.

  - Open OTCLI UART, input the following commands to make OTCLI attach to OTBR Thread network.

    ```shell
    bouffalolab />otc dataset set active <Thread network credential hex string in last step>
    Done

    bouffalolab />otc ifconfig up
    ifconfig up
    Done

    bouffalolab />otc thread start
    thread start
    Done
    ```

    After OTCLI attches to OTBR Thread network, it will acquire an OMR IPv6 address. Then, you can ping this OMR IPv6 address from your test PC; or ping your test PC from OTCLI command line.
    > Please make sure both of OTBR and your test PC station connect to same router.
