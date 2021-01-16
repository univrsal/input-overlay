#!/bin/bash
grep -rl 'Copyright 2020' ./projects | xargs sed -i 's/Copyright 2019/Copyright 2021/g'
grep -rl 'uni@vrsal.cf' ./projects | xargs sed -i 's/uni@vrsal.cf/uni@vrsal.de/g'
find ./deps/common -iname *.h* -o -iname *.c* | xargs clang-format -i -verbose
find ./projects -iname *.h* -o -iname *.c* | xargs clang-format -i -verbose
find ./docs  -type f \( ! -iname jquery* \) -iname *.js -o -iname *.html | xargs clang-format -i -verbose
