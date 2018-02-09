# Perciscope

Analysis tool for descrambling PERCIVAL data taken with Chipscope.


## Compilation

To compile the project:
```
  mkdir build && cd build;
  cmake ..
  make
```
N.B.: ROOT and at least C++11 are required.


## HowTo

The analysis is driven by a XML configuration file which needs:
* Section related to file:
  * Path of input files to analyse
  * Name of files to analyse
  * Extension of files to analyse
  * Number of files to analyse
  * Output path of ROOT file
  * Output path of PNG files
* Section related to the analysis:
  * First bit read
  * Length of data output
  * Number of gain bits
  * Number of fine bits
  * Number of coarse bits

And then to run the project:
```
  ./analyseChipscope -cfg <XML file to use>
```

## TODO

* Complete TODO section!
