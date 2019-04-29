top100
========

`top100` is a tiny external sorting library under 1G memory usage

# Basic

> ./top100 a.urls

This will print the top 100 most frequently occurring urls with count.

Output:

> 1 https://wwww.google.com
>
> 2 https://github.com

# Command line options

> -h [ --help ]         Produce help message
>
> -v [ --version ]      Print version details
>
> -i [ --input ] arg    Input file

# Customization

## Get the top 200 most frequently occurring urls

Edit **counter.hpp:22**, replace the constant 100 with 200 or whatever you want

## Keep the sorted file

The `ExternalSorter::sort` takes a callback arg, which can be used to record the url in order

# Use as a lib

## CMake

> git clone https://github.com/condy0919/top100

Include **top100/sorter.hpp** in your main file:

```cpp
#include "top100/sorter.hpp"
#include <fstream>
#include <iostream>

int main() {
    top100::ExternalSorter sorter{std::ifstream("test")};

    sorter.sort([](std::string s) {
        std::cout << s << '\n';
    });
}
```

Edit **CMakeLists.txt**, let it generate a static top100 library. Replace `add_executable` with `add_library`

Add `add_subdirectory(top100)` in your CMakeLists.txt

Enjoy :)

## Other build systems

DIY

# TODO

- [ ] using aio to write tmp file
- [ ] improve CPU utilization rate
