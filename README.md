# ESP32 Testbed

## Installing

In order to build this project, additional libraries/repositories are needed.

- Espressif ESP-IDF v3.3.2
- Unity test framework v2.5.0
- cppcheck 1.90

In order to test this project, additional packages are needed

- ceedling 0.29.1
- cppcheck 1.90
- gcovr 4.2
- valgrind 3.13.0
- doxygen 1.8.13

To install the third party libraries, type the following

- `git submodule init`
- `git submodule update --recursive`

To install the third party tools, not covered by the `apt install ...` below

- `sudo apt install -y cmake ninja-build gperf flex bison python-pip doxygen graphviz texlive-latex-base gcovr valgrind ruby`
- `gem install ceedling`
- Perform the cppcheck installation described in `docs/building_cppcheck.md`

To install ESP-IDF type:  `./thirdparty/esp-idf/install.sh`

If an error occurs involving python try running the following:

`python -m pip uninstall virtualenv; python -m pip install virtualenv==16.7.9`

An openssl key is required to sign the executable, if one has not been provided type:

`openssl ecparam -name prime256v1 -genkey -noout -out ./esp32/cert/secure_boot_signing_key.pem`

!Keep this openssl key secret!

## Building

###  Setup

The ESP-IDF tools are used for building, to add them to the path properly, use `source ./thirdparty/esp-idf/export.sh` or `. ./thirdparty/esp-idf/export.sh`

### Compilation

In the `esp32` directory, type `idf.py build` to build the project

### Flashing

In the `esp32` directory, type `idf.py flash` to build the project

### Monitoring

In the `esp32` directory, type `idf.py monitor` to build the project

## File Layout
    scripts - Useful project related scripts
    docs - documentation and datasheets
    common - common codebase
        src - source code for control loop
        test - unit tests for control loop
        sim - simulation of control loop
    board - Board-specific codebase
        main - source code for ESP32 and board support
        test - unit tests for board
    thirdparty
        esp-idf - Espressif ESP-IDF v3.3.1
        Unity - Unity test framework v2.5.0
