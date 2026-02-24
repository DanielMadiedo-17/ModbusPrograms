This project consisted of setting up an embedded Riverdi display with an stm32 to be a MODBUS controller while the PIC was programmed to act as a peripheral.

The display was formatted using TouchGFX and allowed for the user to alter the start address and quantity of registers when FC03 was called. It was called on the user pressing a send button.

The development of the STM32 program involved mutex's and Freertos.
A mutex-protected request structure prevents:
-Register start address modification mid-frame
-Quantity mutation during transmission
-Partial frame corruption

Only files directly authored or modified for this project are included.
Vendor-generated boilerplate (CubeMX, TouchGFX auto-generated files, etc.) has been omitted for clarity.

Communication was validated in stages:
-Electrical layer validation using RS-485 transceivers
-Frame inspection using RealTerm
-Protocol verification using Simply Modbus
-Integrated controller–peripheral testing between STM32 and PIC

Observed:
-Correct CRC calculation
-Correct register addressing
-Deterministic response timing
-No frame corruption under concurrent UI updates

