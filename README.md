# cache_measure

A C program that measures cache behaviour using Saavadra and Smith's methodology.

## BUILD & RUN

To build, use `make size`.

To clean up, use `make clean`.

To run a test on your machine, use `make check`.

## METHODOLOGY

The program runs a set of experiments for a group of arrays, whose length is _N_. _N_ increases from _256_ to _256 * (2 ^ 18)_ in the order of 2.

Each experiment consists of 16777216 (2 ^ 24) number of operations. 16777216 is merely a big enough number to reduce noise.

Each operation tries to read an integer from a slot in the array, incease it, and write it back to the slot.

During an experiment, the program performs the first operation on the first slot in the array, then it will jump _STRIDE_ slots and perform the next operation. In the ith experiment, the slot with index (i * STRIDE) mod N will be accessed.

Each experiment for a given _N_ is timed for 16 times. The fastest record _T_ will be taken.

When the size of array (_N_ * sizeof(slot)) exceeds the next level cache size, the read and write performance will drop drastically. A steep increase of _T_ should be observed in the generated graph.

_STRIDE_ value is fixed to 8 in the program. Therefore the program can only be used to measure the cache size of different levels. In order to measure the associativity and cache line size, _STRIDE_ must be made dynamic. A good range would be from 4 to 128 in the order of 2.

## HOW TO MEASURE

*size.png* is a sample performance graph taken from a MacBook Pro (late 2017).

The X axis is the base 2 logarithm value of _N / 256_, where 256 is an arbitrary number so that on a system where integer size is 4 bytes, the smallest array size would be 1kB. The Y axis is the access time _T_ in microseconds.

The first steep increase happens when X is 9. The size of array therefore is 2 ^ 9 * 1kB = 512kB.

The second steep increase happens when X is 12. The corresponding array size is 4MB.

The third steep increase happens when X is 16. The corresponding array size is 64MB.

Therefore, the 3 levels of caches are 512kB, 4MB, and 64MB.

## LIMITATIONS AND OUTLOOK

As above-stated, the program does not measure the cache line size nor the cache associativity. However, it would be easy to implement them. In the future versions, the program will be extended so that it conducts the experiment sets on different _STRIDE_ values.

Specifically, it will conduct the experiment on _STRIDE_ value 4 to 128 in the order of 2. It will then draw a _T_ to _STRIDE_ graph for each given _N_. For a _N_ that is bigger than cache size, when the cache performance stablizes (with a slight decrease in _T_), the _STRIDE_ value is equal to the cache line size.

Similarly, cache associativity can be deduced by _N / STRIDE_, when _T_ stops increasing (for the second time).