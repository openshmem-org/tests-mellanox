tests-mellanox
==============

### Mellanox Vendor Tests for OpenSHMEM API

* Download and Install HPCx package with OpenSHMEM framework from http://bgate.mellanox.com/products/hpcx/
* Check README.txt file from a specific HPCX package for instructions
* Compile and run test kit as following
* In the howto instruction 'module load hpcx' makes visible oshcc compiler needed to build OpenSHMEM application

``` shell
% module load hpcx
% cd verifier
% ./autogen.sh
% ./configure --prefix=$PWD/install
% make install
% oshrun -np 4 $PWD/install/bin/oshmem_test exec --task=all
```
