# ESP32 Testbed

## Installing

In order to build this project, several additiona libraries are needed.

- Espressif ESP-IDF v3.3.1
- Unity test framework v2.5.0

To install the third party libraries, type the following

- `git submodule init`
- `git submodule update --recursive`

The ESP-IDF and doxygen may require additional packages to run.

Under Ubuntu 18.04 LTS run the following to install the missing packages:

`sudo apt install cmake ninja-build gperf flex bison python-pip doxygen graphviz texlive-latex-base`

To install ESP-IDF type:  `./thirdparty/esp-idf/install.sh`

If an error occurs involving python try running the following:

`python -m pip uninstall virtualenv; python -m pip install virtualenv==16.7.9`

An openssl key is required to sign the executable, if one has not been provided type:

`openssl ecparam -name prime256v1 -genkey -noout -out ./esp32/cert/secure_boot_signing_key.pem`

Keep this secret!


## Building

###  Setup

The ESP-IDF tools are used for building, to add them to the path properly, use `source ./thirdparty/esp-idf/export.sh` or `. ./thirdparty/esp-idf/export.sh`

### Compilation

In the `esp32` directory, type `idf.py build` to build the project

### Flashing

In the `esp32` directory, type `idf.py flash` to build the project

### Monitoring

In the `esp32` directory, type `idf.py monitor` to build the project

### Extras

If an issue occurs with


## Tools



## File Layout
    scripts - Useful project related scripts
    docs - documentation and datasheets
    control - control loop codebase
        src - source code for control loop
        test - unit tests for control loop
        sim - simulation of control loop
    board - Board-specific codebase
        main - source code for ESP32 and board support
        test - unit tests for board
    thirdparty
        esp-idf - Espressif ESP-IDF v3.3.1
        Unity - Unity test framework v2.5.0
