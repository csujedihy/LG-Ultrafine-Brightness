# LG-Ultrafine-Brightness

2019-12-29: Using Huawei DP+USB-A to USB-C cable to connect PC Win10 will generate 2 device, so I modified a little.

---

A tool to adjust brightness of LG Ultrafine 4k/5K on Windows without the help of Bootcamp. I built my own PC and made this tool to adjust brightness through my PC.

The program simply send set feature report to the device via USB and then the LG 4/5K monitor adjust the brightness upon receiving such reports.

This project is derived from [https://github.com/unknownzerx/lguf-brightness](https://github.com/unknownzerx/lguf-brightness). 

Thanks @unknownzerx.

The original tool was unable to adjust brightness in Windows 10 and I also made some minor improvements to it like showing current brightness and efficiency while adjusting.

### Linux
Please go to this repo [https://github.com/velum/lguf-brightness](https://github.com/velum/lguf-brightness).

### Usage

```
Press '-' or '=' to adjust brightness.
Press '[' or: ']' to fine tune.
Press 'p' to use the minimum brightness
Press '' to use the maximum brightness
Press 'q' to quit.
```

### Build

1. Open the `sln` file by Visual Studio.
2. Select `x64` configuration.
3. Build!

Note that I put the hidapi.dll in the project and if you don't believe it's secure, you can build a new one (x64) and replace it.
