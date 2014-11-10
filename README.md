tests-mellanox
==============

### Mellanox Vendor Tests for OpenSHMEM API

* Download and Install HPCx package with OpenSHMEM framework from http://bgate.mellanox.com/products/hpcx/
* Check http://bgate.mellanox.com/products/hpcx/README.txt for instructions
* Compile and run test kit as following

``` shell
% module load hpcx
% cd verifier
% ./configure --prefix=$PWD/install
% make install
% oshrun -np 4 $PWD/install/oshmem_test exec --task=all
```
