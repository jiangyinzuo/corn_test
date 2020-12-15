# Corn Test

A mini head-only C/C++ unit test framework. 

## Usage

Just include `corn_test.h`

```c
// example.c
#include "corn_test.h"

TEST(foo) {
    ASSERT(1 < 2)
}

TEST(bar) { 
    ASSERT_EQ(1, 2 - 1)
}

TEST_MAIN()
```

```shell
gcc example.c -o example
./example
```

```
Test start
Running Test foo [1/2]
Running Test bar [2/2]
All tests PASSED!
```