#!/bin/bash



if [ ! -e fde_navi ];then
	echo "Error: fde_navi  not exist. please make it first"
	exit 1
fi

ver=1.0

dst_dir=`ls debian/ -l |grep ^d |awk -F " " '{print $NF}' |tr -d " "`
dst=debian/$dst_dir

tar -cJvpf $dst/fdenavi_${ver}.orig.tar.xz  fde_navi 
cp -a fde_navi $dst/
pushd $dst
dch -i 


#step 5 make debs
sudo dpkg-buildpackage -us -uc
if [ $? != 0 ];then
	echo "Error: make deb failed."
	popd
	exit 1
fi
popd 
echo "deb file generated successfully"


