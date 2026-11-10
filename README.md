# Tanuki - A temperature/humidity information system

## Matériel (BOM)

| Quantité | Repère | Désignation |
|---|---|---|
| 1 | U1 | Carte NUCLEO-F401RE |
| 3 | LED1-LED3 | LED rouge 5 mm |
| 3 | R1-R3 | Résistance 330 Ω |
| 3 | SW1-SW3 | Bouton poussoir tactile 6×6 mm |

---

## Pinout
   Signal | Broche | Connecteur | Direction | Actif |
 |--------|--------|------------|-----------|-------|
 | LED0 verte (LD2, embarquée) | PA5 | CN10-11 | Sortie | Haut |
 | LED1 verte externe | PC11 | CN7-2 | Sortie | Haut |
 | LED2 rouge | PB5 | CN10-29 | Sortie | Haut |
 | LED3 bleue | PC3 | CN7-37 | Sortie | Haut |
 | SW1 droite | PC4 | CN10-34 | Entrée (pull-up) | Bas (appuyé) |
 | SW2 milieu | PB14 | CN10-28 | Entrée (pull-up) | Bas (appuyé) |
 | SW3 gauche | PB13 | CN10-30 | Entrée (pull-up) | Bas (appuyé) |

---

## How to flash the microcontroller

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

## Comment deboguer le firmware tournant sur un microcontroleur.

Une facon de faire est la suivante.

Premièrement et avant toute chose, n'oublions pas de programmer le microcontroleur avec un firmware ayant été compilé avec des flags de debug.

Commençons par démarrer le serveur OpenOCD avec ST-LINK pour STM32F4.
Par default, OpenOCD écoute sur `locahost:3333`.
```sh
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
```

Lançons gdb en parallèle.
```sh
arm-none-eabi-gdb <path/to/elf>
```

Dans gdb, connectons-nous à la cible et chargeons le firmware.
```gdb
target remote localhost:3333  # se connecte au serveur OpenOCD
monitor reset halt            # réinitialise la cible et la maintient à l’arrêt
load                          # charge le programme sur la cible
```

Imaginons que nous voulons vérifier la valeur d'une variable.
Dans le code du firmware, nous pourrions avoir ceci.
```c++
volatile uint32_t pc4Raw = (GPIOC->IDR >> 4U) & 1U;
```
Ainsi, dans gdb, lorsque nous arriverions à cette instruction, nous afficherions la valeur de cette variable.
```gdb
print pc4Raw
```

Pour "redémarrer le microcontroleur" sans le reprogrammer (et dans gdb).
```gdb
monitor reset halt
continue
```

Voici un cas simple mais concrèt de l'aide que m'a procuré gdb.
Ces commandes m'ont été utiles lors du bring-up d'un interrupteur.
```gdb
print /x (GPOIC->PUPDR >> 8) & 3
print /x (GPOIC->MODER >> 8) & 3
print /x (GPOIC->IDR >> 4) & 1
```
Celles-ci m'ont permis de mettre en avant une mauvaise configuration de la broche. En effet, le mode était configuré en sortie au lieu d'entrée.
