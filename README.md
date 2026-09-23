# Tanuki - A temperature/humidity information system

Install the ARM official cross-compiler:
```sh
mkdir -p /$HOME/.local/opt
cd /$HOME/.local/opt
wget https://developer.arm.com/-/media/Files/downloads/gnu/13.3.rel1/binrel/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz
tar xf arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz
rm arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz
cd -
```

Add the following directory the PATH environment (.zshrc for example)
```sh
export ARM_GNU_TOOLCHAIN="$HOME/.local/opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi"
export PATH="$ARM_GNU_TOOLCHAIN/bin:$PATH"
```

To configure and build the project and directly use the CMake preset using the following commands.
```sh
# Configure
cmake --preset Debug
# Build
cmake --build --preset Debug
```

To flash the microcontroller the following command can be issued.
```sh
openocd -f interface/stlink.cfg \
	-f target/stm32f4x.cfg \
	-c "program ./firmware_path verify reset exit"
```

The following command gives access to a shell connected to the mcu using the uart protocol (if effectively configured on the mcu).
```sh
picocom -b 115200 --parity=n --databits=8 --stopbits=1 --flow=n /dev/ttyACM0
```
