rm -fr ../objs/ds/
mkdir ../objs/ds
cp -r ./ds/modules.cfg ../objs/ds/
cd ds
make -j -f buildlib.mk
cd ..

