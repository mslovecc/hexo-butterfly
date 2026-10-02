#if 0
#!/usr/bin/env bash
tmpfile=$(mktemp)
function rm_tmpfile {
  if [ -f "$tmpfile" ]; then
    rm -f "$tmpfile"
  fi
}
trap 'trap - EXIT; rm_tmpfile; exit -1' INT PIPE TERM
gcc -Os ${0}  -lm -pthread -lstdc++ -lasound -o $tmpfile
if [ $? -ne 0 ]; then
	exit -1
fi
$tmpfile "$@"
ret=$?
rm_tmpfile
exit $ret
#endif
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <vector>
#include <cmath>
#include <thread>
static const double pi = 3.141592653589793;
using namespace std;
#include <alsa/asoundlib.h>
#include <time.h>
#include <unistd.h>

uint64_t now(){
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC,&ts);
	uint64_t ps = uint64_t(ts.tv_sec)*1000ULL*1000ULL*1000ULL*1000ULL+uint64_t(ts.tv_nsec)*1000ULL;
	if(ps==0ULL)
		ps=1ULL;
	return ps;
}
	
int main(int argc, char *argv[]){
	int sinhz= 44100;
	uint64_t sintest;
	bool sinwave = true;
	string devname = "hw:0,0";
	if(argc>1){
		devname = argv[1];
	}
	if(argc>2){
		sinhz = strtol(argv[2], nullptr, 10);
	}
	if(argc>3){
		sinwave = false;
		sintest = strtoull(argv[3], nullptr, 16);
	}
	snd_pcm_t* hndl;
	int ret = snd_pcm_open(&hndl, devname.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
    if(ret != 0) {
    	cout <<"err snd_pcm_open "<<devname<<endl;
		return -1;
    }

	snd_pcm_hw_params_t *hw_params; 
	snd_pcm_hw_params_alloca(&hw_params);
	snd_pcm_hw_params_any(hndl, hw_params);
	
	if(ret=snd_pcm_hw_params_set_access(hndl, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)!=0){
    	cout <<"err snd_pcm_hw_params_set_access"<<endl;
		return -1;
	}
	if(ret=snd_pcm_hw_params_set_format(hndl, hw_params, SND_PCM_FORMAT_S32_LE)!=0){
    	cout <<"err snd_pcm_hw_params_set_format"<<endl;
		return -1;
	}
	if(ret=snd_pcm_hw_params_set_rate(hndl, hw_params, sinhz, 0)!=0){
    	cout <<"err snd_pcm_hw_params_set_rate "<<sinhz<<endl;
		return -1;
	}
	if(ret=snd_pcm_hw_params_set_channels(hndl, hw_params, 2)!=0){
    	cout <<"err snd_pcm_hw_params_set_channels"<<endl;
		return -1;
	}
	//3.3333msec
	int AlsaPeriodSize = sinhz/300;
	//30msec
	int AlsaPeriodCount = 9;
	//30msec
	if(ret=snd_pcm_hw_params_set_periods(hndl, hw_params, AlsaPeriodCount, 0)!=0){
    	cout <<"err snd_pcm_hw_params_set_periods "<<AlsaPeriodCount<<endl;
		return -1;
	}
	
	if(ret=snd_pcm_hw_params_set_period_size(hndl, hw_params, AlsaPeriodSize,0)!=0){
    	cout <<"err snd_pcm_hw_params_set_period_size"<<endl;
		return -1;
	}

	if(ret=snd_pcm_hw_params(hndl, hw_params)!=0){
    	cout <<"err snd_pcm_hw_params"<<endl;
		return -1;
	}
	snd_pcm_sframes_t AlsaMaxPos = snd_pcm_avail_update(hndl);
	vector<uint8_t> tmp;
	tmp.resize(AlsaMaxPos*4*2);
	double rad=0;
	auto writeSin = [&](){
		if(!sinwave){
			for(int a=0;a<AlsaMaxPos;++a){
				uint64_t* pp =  ((uint64_t*)&tmp[a*4*2]);
				*pp=sintest;
			}
		}else{
			for(int a=0;a<AlsaMaxPos;++a){
				uint64_t* pp =  ((uint64_t*)&tmp[a*4*2]);
				//32bit 2ch
				uint32_t da=int32_t(sin(rad)*0x7FFFFFFF);
				rad+=(1000.0/(double)sinhz)*2.0*pi;
				
				*pp =  (((uint64_t)da)<<32UL)|((uint64_t)da);
			
			}
			
		}	
		
		snd_pcm_sframes_t writecount = snd_pcm_writei(hndl,&tmp.front() ,AlsaMaxPos);
		if(writecount!=AlsaMaxPos){
			cout << "error write "<<writecount<<endl;
			snd_pcm_recover(hndl,writecount,0);
			return false;
		}
		return true;
	};
	

	
	writeSin();
	snd_pcm_start(hndl);
	uint64_t lastime = now();
	size_t count = 0;
	bool ondelay = false;
	while(true){
		writeSin();
		if(ondelay){
			this_thread::sleep_for(chrono::milliseconds(40));
			ondelay = false;
		}

		++count;
		uint64_t ntime = now();
		if(ntime-lastime >= 1ULL*1000ULL*1000ULL*1000ULL*1000ULL){
			
			cout << " "<<double(count*AlsaMaxPos*32)/double(ntime-lastime)*(1ULL*1000ULL*1000ULL*1000ULL*1000ULL)/1000ULL/1000ULL << " Mbps/ch";
			cout << " "<<double(count*AlsaMaxPos)/1000.0<<"kHz" <<endl;
			lastime = ntime;
			
			count=0;
		}
	}
		
	return 0;
}
