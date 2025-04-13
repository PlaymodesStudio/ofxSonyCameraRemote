#!/bin/bash
# Script to fix library paths in Sony Camera Remote SDK dylibs

cd libs/CRSDK/lib

# Fix the install name in libmonitor_protocol.dylib
echo "Fixing libmonitor_protocol.dylib..."
install_name_tool -id "@rpath/libmonitor_protocol.dylib" libmonitor_protocol.dylib

# Fix the install name in libmonitor_protocol_pf.dylib
echo "Fixing libmonitor_protocol_pf.dylib..."
install_name_tool -id "@rpath/libmonitor_protocol_pf.dylib" libmonitor_protocol_pf.dylib

# Fix the install name in libCr_Core.dylib
echo "Fixing libCr_Core.dylib..."
install_name_tool -id "@rpath/libCr_Core.dylib" libCr_Core.dylib

# Fix dependencies between libraries
echo "Fixing dependencies between libraries..."
install_name_tool -change "/Users/muramatsu/Projects/sony/_repository/Network-VericRxLibrary_231H/RxLib/core/build/bin/macosX64/releaseShared/libmonitor_protocol.dylib" "@rpath/libmonitor_protocol.dylib" libCr_Core.dylib
install_name_tool -change "/Users/muramatsu/Projects/sony/_repository/Network-VericRxLibrary_231H/RxLib/core/build/bin/macosArm64/releaseShared/libmonitor_protocol.dylib" "@rpath/libmonitor_protocol.dylib" libCr_Core.dylib

# Fix dependencies for all libraries in CrAdapter
cd CRSDK/CrAdapter
echo "Fixing CrAdapter libraries..."
for lib in *.dylib
do
  echo "Fixing $lib..."
  install_name_tool -id "@rpath/$lib" "$lib"
  install_name_tool -change "/Users/muramatsu/Projects/sony/_repository/Network-VericRxLibrary_231H/RxLib/core/build/bin/macosX64/releaseShared/libmonitor_protocol.dylib" "@rpath/libmonitor_protocol.dylib" "$lib"
  install_name_tool -change "/Users/muramatsu/Projects/sony/_repository/Network-VericRxLibrary_231H/RxLib/core/build/bin/macosArm64/releaseShared/libmonitor_protocol.dylib" "@rpath/libmonitor_protocol.dylib" "$lib"
done

echo "All library paths fixed!"