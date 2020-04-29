#!/bin/sh
echo "Running cppcheck"
cppcheck --quiet -ibuild -ibuild_ceedling -itest -I../thirdparty/esp-idf/components --suppress=*:../thirdparty/* --addon=report/misra/misra.json --language=c --enable=all --xml --xml-version=2 . 2> report/misra/cppcheck-result.xml

echo "Building cppcheck HTML report in report/misra"
../thirdparty/cppcheck/htmlreport/cppcheck-htmlreport --report-dir=report/misra --file=report/misra/cppcheck-result.xml --title=Testbed --source-dir=.

echo "Running ceedling unit tests"
ceedling clobber; ceedling test:all; ceedling gcov:all utils:gcov

echo "Copying coverage report into report/coverage"
cp build_ceedling/artifacts/gcov/* report/coverage/
