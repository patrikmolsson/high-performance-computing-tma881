# Assignment 0
We downloaded the plasma installer from <http://icl.cs.utk.edu/projectsfiles/plasma/pubs/plasma-installer.tar.gz>, decompressed it. With installer followed the python script setup.py. The script requires some specifications, c/fortran compilers and the locations of the BLAS and LAPack libraries. Ozzys OS is actually displayed upon login but it can also be found in `/proc/version´. The packages could then be found using 
 These were already on the system and we located them with
cat /proc/version
gave the OS. 
| grep libblas
dpkg -L libblas3
dpkg -L liblapack-dev

./setup.py --cc=gcc --fc=gfortran --blaslib=/usr/lib/libblas/libblas.so --lapacklib=/usr/lib/liblapack.so --notesting

FIND BLAS
dpkg -L libblas3

FIND LAPACK
dpkg -L liblapack-dev
