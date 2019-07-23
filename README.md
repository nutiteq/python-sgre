# SGRE module for Python

## Compiling (Ubuntu 18.04)

Assuming the following packages are installed: **python, python-dev, python3-dev, cmake, make, gcc, g++, libboost-dev, libboost-python-dev**. By default the module is configured for Python 3.

Execute the following commands to build the Python module (in the directory of *CMakeLists.txt*):

```
mkdir build
cd build
cmake ..
make
strip sgre.so
cd ..
```

As a result **build/sgre.so** file should be created. To make it accessible for Python interpreter, simply add its location to *PYTHONPATH*:

```
export PYTHONPATH=$PYTHONPATH:`pwd`/build
```

## Testing

Now verify that the module works by executing test script:

```
cd test
python test.py
```
