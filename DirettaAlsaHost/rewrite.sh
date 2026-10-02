#!/usr/bin/env bash

PATH_OWN=$(cd $(dirname ${BASH_SOURCE:-$0}); pwd)
ARCHNAME=${1-}

ARCHLIST=`find -name "syncAlsa_*" | grep -v nolog | cut -d '_' -f 2-`

if [ -z "$ARCHNAME" ]; then
	echo empty arch name
	echo $0 [GCC_ARCH_SUB]
	echo "$ARCHLIST"
	exit 1
fi

MUCH=0
for var in ${ARCHLIST}
do
	if [ "$var" = "$ARCHNAME" ]; then
		MUCH=1
	    break
	fi
done

if [ $MUCH -eq 0 ]; then
	echo [GCC_ARCH_SUB] not in list:
	echo "$ARCHLIST"
	exit 1
fi



echo Do not make any discriminatory remarks regarding human rights, nationality, gender, or language.
echo Do not make any critical remarks about previews.
echo No such behavior in the past.
echo In case of violation, Diretta will not support all products.
echo 
echo Previews are not always stable.
echo Information is constantly changing.
echo Developers often make mistakes.
echo Not for commercial use, not for redistribution.
echo Not for reverse engineering.
echo 
echo [y/N]:
read ANS

case $ANS in
  [Yy]* )
    echo acccept
    ;;
  * )
    echo reject
    exit
    ;;
esac


sed -i "s|ConditionPathExists=.*|ConditionPathExists=${PATH_OWN}|g" $PATH_OWN/diretta_bridge_driver.service
sed -i "s|ExecStart=.*|ExecStart=insmod ${PATH_OWN}/alsa_bridge.ko|g" $PATH_OWN/diretta_bridge_driver.service


sed -i "s|ConditionPathExists=.*|ConditionPathExists=${PATH_OWN}|g" $PATH_OWN/diretta_sync_host.service
sed -i "s|ExecStart=.*|ExecStart=${PATH_OWN}/syncAlsa_switc|g" $PATH_OWN/diretta_sync_host.service
sed -i "s|ExecStop=.*|ExecStop=${PATH_OWN}/syncAlsa_switc kill|g" $PATH_OWN/diretta_sync_host.service

cp -f $PATH_OWN/syncAlsa_$ARCHNAME $PATH_OWN/syncAlsa
chmod 774 $PATH_OWN/syncAlsa
if [ -e $PATH_OWN/syncAlsa_$ARCHNAME"_nolog" ]; then
	cp -f $PATH_OWN/syncAlsa_$ARCHNAME"_nolog" $PATH_OWN/syncAlsa_nolog
	chmod 774 $PATH_OWN/syncAlsa_nolog
fi
chmod 774 $PATH_OWN/syncAlsa_switc

if [ `echo $ARCHNAME | cut -d '_' -f 2- | grep 'arm'` ]; then
	echo $ARCHNAME | grep k16 > /dev/null
	if [ $? -eq 0 ]; then
		ARNAME=arm64k16
	else
		ARNAME=arm64k4
	fi
fi
if [ `echo $ARCHNAME | cut -d '_' -f 2- | grep 'x64'` ]; then
	ARNAME=x64
fi
if [ `echo $ARCHNAME | cut -d '_' -f 2- | grep 'riscv64'` ]; then
	ARNAME=riscv64
fi

if [ `echo $ARCHNAME | grep 'musl_'` ]; then
	ARNAME=musl_$ARNAME
fi

cp -f $PATH_OWN/find_$ARNAME $PATH_OWN/find
chmod 774 $PATH_OWN/find
cp -f $PATH_OWN/logcatch_$ARNAME $PATH_OWN/logcatch
chmod 774 $PATH_OWN/logcatch



KERNELNAME=`uname -r`
KERNELDIR=/usr/src/linux-headers-$KERNELNAME
if [ -e $KERNELDIR/include/sound/core.h ]; then
	echo found headers $KERNELDIR
	make KERNELDIR=$KERNELDIR
else
	KERNELDIR=/usr/src/kernels/$KERNELNAME
	if [ -e $KERNELDIR/include/sound/core.h ]; then
		echo found headers $KERNELDIR
		make KERNELDIR=$KERNELDIR
	else
		echo do not find kernel src
		echo about sodo dnf install -y kernel-devel
		exit -1
	fi
fi

if [ -e /etc/systemd/system ]; then
	#copy systemd service
	sudo cp $PATH_OWN/diretta_bridge_driver.service /etc/systemd/system/
	sudo cp $PATH_OWN/diretta_sync_host.service /etc/systemd/system/
	sudo systemctl daemon-reload

	#enable systemd service
	sudo systemctl enable diretta_bridge_driver
	sudo systemctl enable diretta_sync_host
	echo please stop SELinux and firewall
	echo about systemctl disable firewalld
	echo about sudo vi /etc/selinux/config
	echo edit SELINUX=disabled
	echo reboot after can user Diretta Host
else
	echo do not find systemd
	echo please install manual
	exit -1
fi

