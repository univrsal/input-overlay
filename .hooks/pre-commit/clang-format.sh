#!/bin/sh
grep -rl 'Copyright 2019' ./io-obs | xargs sed -i 's/Copyright 2019/Copyright 2020/g'
grep -rl 'Copyright 2019' ./io-cct/src | xargs sed -i 's/Copyright 2019/Copyright 2020/g'
grep -rl 'Copyright 2019' ./io-client/src | xargs sed -i 's/Copyright 2019/Copyright 2020/g'
find ./io-obs -iname *.h* -o -iname *.c* | xargs clang-format -style=WebKit -i -verbose
find ./io-cct/src -iname *.h* -o -iname *.c* | xargs clang-format -style=WebKit -i -verbose
find ./io-client/src -iname *.h* -o -iname *.c* | xargs clang-format -style=WebKit -i -verbose
