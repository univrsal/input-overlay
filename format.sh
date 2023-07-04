#!/bin/bash
find ./deps/common -iname *.h* -o -iname *.c* | xargs clang-format -i -verbose
find ./client/src -iname *.h* -o -iname *.c* | xargs clang-format -i -verbose
find ./src -iname *.h* -o -iname *.c* | xargs clang-format -i -verbose
find ./docs -type f ! -iname "jquery-3.5.1.min.js" -name "*.js" -o -name "*.html" ! -path "css" | xargs clang-format -i -verbose

