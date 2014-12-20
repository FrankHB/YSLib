#!/usr/bin/sh
# (C) 2014 FrankHB.
# Script to create Android debug keystore.

set -e

is_in_path()
{
	cmd=$1
	path=$2
	result=1

	echo $cmd
	echo $path
	echo $result

	oldIFS=$IFS
	IFS=":"
	for dir in $path
	do
		if [ -x $dir/$cmd ]; then
			result=0
		fi
	done
	IFS=oldifs
	return $result
}

java_exe=`$ANDROID_SDK/tools/lib/find_java`

if [ `$(is_in_path "cygpath")` -ne 0 ]
then
	java_d=`cygpath -d "$java_exe"`
	export JAVA=`cygpath "$java_d"`
else
	export JAVA="$java_exe"
fi

rm -f ~/.android/debug.keystore
mkdir -p ~/.android
$(dirname $JAVA)/keytool -sigalg MD5withRSA -keyalg RSA -keysize 1024 -genkey -v -keystore ~/.android/debug.keystore -storepass android -alias androiddebugkey -keypass android -dname "CN=Android Debug,O=Android,C=US"

echo Done.

