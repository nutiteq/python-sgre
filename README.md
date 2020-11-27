# SGRE module for Python

This project provides Python module for Simple GeoJSON-based routing engine (SGRE).

Simple GeoJSON-based routing engine provides a routing engine specially designed for blueprints/graphs defined via GeoJSON input. The routing engine supports different routing profiles (speeds, restrictions), supports height/3D routing (by utilizing Z coordinate). Both line and polygon geometry (both non-convex polygons and polygons with holes) can be used, including mixed line and polygon geometry.

SGRE was originally designed for [CARTO Mobile SDK](https://github.com/CartoDB/mobile-sdk). Its documentation is available in [Wiki](https://github.com/CartoDB/mobile-sdk/wiki/SGRE-configuration).

## Compiling (Ubuntu 18.04)

Assuming the following packages are installed: **python, python-dev, python3-dev, cmake, make, gcc, g++, libboost-dev, libboost-python-dev**. By default the module is configured for Python 3.

Check the version of the installed Python interpreter:

```
python3 --version
```

Modify *PYTHON_VERSION* and *BOOST_PYTHONLIB* variables in *CMakeLists.txt* to match the version of the installed Python interpreter. For example (for Python 3.6.x):

```
set(PYTHON_VERSION 3.6)
set(BOOST_PYTHONLIB python-py36)
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

Now verify that the module works by executing test script:

```
cd test
python3 test.py
```
