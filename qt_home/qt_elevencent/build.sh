#!/bin/bash
usage(){
    echo "usage: <$0> <qt-path> <plantform(gcc/android_arm64_v8a/android_armv7)> <android-sdk-path> <ndk-path> "
}
if [ $# -lt 1 ] ; then
    usage
    exit 1
fi
qt_path=$1
plantform=""
android_sdk_path=""
android_ndk_path=""
cmake_str=""
if [ $# -eq 1 ] ; then
    plantform="gcc_64"
else
    plantform=$2    
fi
source_dir=`dirname $0`
build_dir=${source_dir}/build_${plantform}
mkdir $build_dir > /dev/null 2>&1

if [[ $plantform == "gcc_64" ]] ; then
    cmake_str="cmake -S ${source_dir} -B ${build_dir}  -DQT_PLANTFORM=${plantform} -DQT_PATH=${qt_path} -DCMAKE_PREFIX_PATH=${qt_path}/${plantform}/lib/cmake"
elif [[ $plantform =~ "android" ]] ; then
    if [ $# -lt 4 ] ; then
	usage
	exit 1
    fi
    android_sdk_path=$3
    android_ndk_path=$4
    cmake_str="cmake -S ${source_dir} -B ${build_dir} -DQT_PLANTFORM=${plantform} -DQT_PATH=${qt_path} -DCMAKE_PREFIX_PATH="${qt_path}/${plantform}/lib/cmake" -DANDROID_STL:STRING=c++_shared -DANDROID_NDK:PATH=${android_ndk_path} -DANDROID_ABI:STRING=arm64-v8a -DCMAKE_C_COMPILER:STRING=${android_ndk_path}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang -DCMAKE_CXX_COMPILER:STRING=${android_ndk_path}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ -DCMAKE_TOOLCHAIN_FILE:PATH=${android_ndk_path}/build/cmake/android.toolchain.cmake -DCMAKE_FIND_ROOT_PATH:PATH=${qt_path}/${plantform} -DCMAKE_MODULE_PATH:STRING=${qt_path}/${plantform} -DQT_HOST_PATH:STRING=${qt_path}/gcc_64 -DANDROID_SDK_ROOT:PATH=${android_sdk_path} -DANDROID_NATIVE_API_LEVEL:STRING=23 -DANDROID_PLATFORM=android-23"
else
    usage
    exit 1
fi
echo $cmake_str
chmod 777 -R ${build_dir}
rm ${build_dir}/* -rf
ret=`$cmake_str`
cd ${build_dir}
make  -j `cat /proc/cpuinfo|grep processor|wc -l`
