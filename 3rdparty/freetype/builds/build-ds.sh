#!/usr/bin/env sh
# (C) 2014, 2020-2021 FrankHB.
# FreeType2 build script for platform DS.

rm -fr ../objs/ds/ && mkdir -p ../objs/ds \
&& cp -r ./ds/modules.cfg ../objs/ds/ && (cd ds && make -j -f buildlib.mk)

