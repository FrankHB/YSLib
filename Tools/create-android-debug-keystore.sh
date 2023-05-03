#!/usr/bin/env sh
# (C) 2014-2015, 2020, 2023 FrankHB.
# Script to create Android debug keystore.

set -e

is_in_path()
{
	cmd="$1"
	path="$2"
	result=1

	echo "$cmd"
	echo "$path"

	saved_IFS="$IFS"

	IFS=:
	for dir in $path
	do
		if test -x "$dir/$cmd"; then
			result=0
		fi
	done
	IFS="$saved_IFS"
	return $result
}

java_exe=$("$ANDROID_SDK/tools/lib/find_java")

if is_in_path "cygpath"; then
	java_d=$(cygpath -d "$java_exe")
	JAVA=$(cygpath "$java_d")
else
	JAVA="$java_exe"
fi
export JAVA

rm -f ~/.android/debug.keystore
mkdir -p ~/.android
"$(dirname "$JAVA")/keytool" -sigalg MD5withRSA -keyalg RSA -keysize 1024 \
	-genkey -v -keystore ~/.android/debug.keystore -storepass android -alias \
	androiddebugkey -keypass android -dname "CN=Android Debug,O=Android,C=US"

echo Done.

