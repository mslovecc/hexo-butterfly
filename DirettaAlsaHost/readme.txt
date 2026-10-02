
Synchro Sync Alsa

Do not make any discriminatory remarks regarding human rights, nationality, gender, or language
Do not make any critical remarks about previews
No such behavior in the past.
In case of violation, Diretta will not support all products.

Previews are not always stable.
Information is constantly changing
Developers often make mistakes.
Not for commercial use, not for redistribution
Not for reverse engineering.


[build install]
./rewrite.sh  [ARCH NAME]

[test]
modprobe snd_pcm
insmod xxxx/driver/alsa_bridge.ko
xxxx/syncAlsa [config file name] [product unique name]

[example] : multi target ,  target profile 
sudo syncAlsa_XXXXXXX setting.inf


[Receive logs]
logcatch


[install]

#copy systemd service
sudo cp ./diretta_bridge_driver.service /etc/systemd/system
sudo cp ./diretta_sync_host.service /etc/systemd/system

#enable systemd service
sudo systemctl enable diretta_bridge_driver
sudo systemctl enable diretta__host

reboot after can user Diretta Host


[setting]

ThredMode 	BIT OR
	Critcal=1
	 Raise the priority of the outgoing thread from High to Critcal.
	NoShotrSleep=2
	 Busy-wait if the time until the next transmission is less than 100us.
	NoSleep4Code=4
	 Busy-wait when the number of CPU cores is 4 or more (do not sleep when CPU 4 or more)
	SocketNoBlock=8
	 Do not send blocked sockets (do not wait for transmission to complete before proceeding)
	OccupiedCPU=16
	 Occupied CPU core number 3 or 2
	FEEDBACK=32,64,128 (moving average)
	 Use a moving average to calculate the feedback that determines the transmission cycle (1 to 7)
	NOFASTFEEDBACK=256
	 Stops the process of temporarily sending more packets for retransmissions, etc.
	IDLEONE=512
	 Turn busy wait (no sleep) into IDLE processing (sleep 0), limited to once between transmissions.
	IDLEALL=1024
	 IDLE processing (Sleep0) with Busy Weighting (not Sleep)
	NOSLEEPFORCE=2048
	 Force Busy Weighting
	LIMITRESEND=4096
	 Limit the retransmission buffer size from 1000 milliseconds to 100 milliseconds.
	NOJUMBOFRAME=8192
	 Not use of Jumbo Frames
	NOFIREWALL=16384
	 Do not send a single packet per connection to train the firewall.
	NORAWSOCKET=32768
	 Suppresses MS Mode 3 operation.

InfoCycle
	Information Packet Cycle Time microsecond

TargetProfileLimitTime
	Target Profile using 0=SelfProfile other=LimitCycleTime
		it profile is automatically change for your system.
		If diretta drops process to high load then, fall down to light diretta processing.
		
FlexCycle
	diable is Fix Cylemode
	enable is Flex Cyclemode
	random is Random Cyclemode
	max is Flex Cyclemode Packt Full

CycleTime
	Transfer Packet Cycle Maximum Time microsecond

CycleMinTime
	Transfer Packet Cycle Minimum Time microsecond
	onry random mode

 (only grobale)
Debug
	disable not output
	enable is put out debug message to logcatch
	stdout is put out debug message to logcatch/stdout
	nolog is without logging function app ( only if a corresponding binary exists
Interface
	connect interface lits sepalete(,)  eth1,eth2 
	
Broadcast
	When a target is detected, the destination MAC address of the multicast packet is spoofed to appear as a broadcast address.
	It may be possible to trick switches that do not forward IPv6 multicast traffic
	enable is add Broadcast packet
	nomulticast is use Broadcast , not use Multicast
	
ScanOnlineStop
	Stop scanning for targets during playback

ScanInterval
	Target scan interval Ttime second
	default 20 sec
	
periodMax
	alsa buffer period max
periodMin
	alsa buffer period min
periodSizeMax
	alsa buffer size max
periodSizeMin
	alsa buffer size min
(minimumsize = periodMin*periodSizeMin )
(maxmumsie = periodMin*periodSizeMax )
CpuSend
	Use with OccupiedCPU(ThredMode 16)
	Fixes the CPU used by the send thread.
	Range setting.
	XX-XX (0-3  CPU 0,1,2,3
	From among them, the transmission thread is assigned to the open CPU.
CpuOther
	Use with OccupiedCPU(ThredMode 16)
	Fixes the CPU used for non send threads.
	Range setting.
	XX-XX (0-3  CPU 0,1,2,3
	Assign threads to those CPUs.
	
syncBufferCount
	Buffer count between ALSA Bridge and packet transmission
	How many of the specified periodSizen will be allocated as a buffer when the player app opens ALSA
	If set to 0, packets are created directly from ALSA memory.
	Since notifications are also sent to the Alsa thread, this may have an impact depending on the transmission cycle.
	default 8
	
alsaUnderrun
	Silent transport when buffer underrun occurs due to player app write delay.
	In the case of DISABLE, recovery may be difficult due to continued playback
	default enable

alsaUnderrunSleep
	Sleep for a specified duration (microseconds) when a player buffer underrun is detected
	The syncBufferCount must be long enough
	
alsaUnderrunClear
	Clears the loaded ALSA buffer using mute
	
unInitMemDet
	If the same content is written from the application to alsa, the part is replaced by mute.
	It cannot be a complete solution for uninitialized memory.
	Problem inherently solved by the player.

LatencyBuffer
	Try to change the buffer size of Target
	If 0, Target's standard time is used.
	Time microsecond
	
disConnectDelay
	Alsa Close does not wait for the connection with the Target to terminate.
	If you start playback immediately after closing,
	default enable
	
singleMode
	Operate only for a single DirettaTarget
	Even if the target encounters an error, it maintains Alsa and emulates behavior like a fixed sound card.
	Therefore, if there are changes to the Target address or similar, recovery is not possible.
	The setting value is the first octet displayed in the Target list view.
	syncAlsa --find
	
	
	
inf group
[global] or none
	defalt profile

[XXXX] 
	XXXX = TargetName
	custum Target profile
