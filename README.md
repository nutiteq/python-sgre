# SGRE module for Python

## Compiling (Ubuntu 16.04)

Assuming the following packages are installed: **python, python-dev, cmake, make, gcc, g++, libboost-dev, libboost-python-dev**. By default the module is configured for Python 2.7.
To use it with Python 3.x, make sure that these additional packages are installed: **python3, python3-dev**. Also, you need to update the following lines in *CMakeLists.txt*:

```
set(PYTHON_VERSION 3.5)
set(BOOST_PYTHONLIB python-py35)
```

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

Now verify that the module works by executing test script (use `python3` if using Python 3):

```
cd test
python test.py
```
