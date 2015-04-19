set -e

. scripts/include.sh


# installer
cd ~/deps/Californium
rm -rf nsis
mkdir nsis
git archive HEAD | tar -x -C nsis
cd nsis/src
mkdir ../release
cp ../../release/* ../release/
cp ../../src/*.exe .
makensis ../share/setup.nsi
rm -rf $OUTDIR/setup
mkdir $OUTDIR/setup
cp ../share/Californium-*-win32-setup.exe $OUTDIR/setup/

# results
cd $OUTDIR
rm -rf Californium-dist.zip
zip -r Californium-dist.zip setup client daemon

echo -e "\n\n"
echo "Results are in $OUTDIR/Californium-dist.zip"
echo -e "\n"


