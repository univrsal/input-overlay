#!/bin/bash
grep -rl 'Copyright 2019' ./projects | xargs sed -i 's/Copyright 2019/Copyright 2020/g'
find ./deps/common -iname *.h* -o -iname *.c* | xargs clang-format -i -verbose
find ./projects -iname *.h* -o -iname *.c* | xargs clang-format -i -verbose
