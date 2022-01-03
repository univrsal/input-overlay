#!/bin/bash
grep -rl 'Copyright 2021' ./projects | xargs sed -i 's/Copyright 2021/Copyright 2022/g'
grep -rl 'uni@vrsal.de' ./projects | xargs sed -i 's/uni@vrsal.de/uni@vrsal.xyz/g'
find ./deps/common -iname *.h* -o -iname *.c* | xargs clang-format -i -verbose
find ./projects -iname *.h* -o -iname *.c* | xargs clang-format -i -verbose
find ./docs -type f ! -iname "jquery-3.5.1.min.js" -name "*.js" -o -name "*.html" ! -path "css" | xargs clang-format -i -verbose

