#!/bin/bash

set -e

##############################函数定义############################## 
SYS_GetOnceInfo()
{
	echo cpu num:`grep -c "model name" /proc/cpuinfo` 
}

#uptime
SYS_GetUptime()
{
	echo ""
	echo uptime:
	GetDay=`uptime |awk '{print $4}'`
	if [ $GetDay == "days," ]; then
		GetDay=`uptime |awk '{print $3}'`
		GetTime=`uptime |awk '{print $5}'`
		GetMin=`uptime |awk '{print $6}'`
		if [ $GetMin == 'min,' ]; then
			echo $GetDay days ${GetTime//,/ } min
		else
			echo $GetDay days ${GetTime//,/ }
		fi
	else
		GetTime=`uptime |awk '{print $3}'`
		GetMin=`uptime |awk '{print $4}'`
		if [ $GetMin == 'min,' ]; then
			echo 0 days ${GetTime//,/ } min
		else
			echo 0 days ${GetTime//,/ }
		fi

	fi

}

#cpu
SYS_GetCpuInfo()
{
	echo ""
	echo "cpu:"
	#用户空间占用cpu百分比
	echo us:`top -b -n 1 | grep Cpu | awk '{print $2}' `%

	#用户内核占用cpu百分比
	echo sy:`top -b -n 1 | grep Cpu | awk '{print $4}' `%

	#用户空闲占用cpu百分比
	echo ni:`top -b -n 1 | grep Cpu | awk '{print $6}' `%
}

#mem
SYS_GetMemInfo()
{
	echo ""
	echo "mem:"
	#总量
	GetTotal_G=`free -hm|grep "Mem"|awk '{print $2}'`
	GetUsed_G=`free -hm|grep "Mem"|awk '{print $3}'`
	GetFree_G=`free -hm|grep "Mem"|awk '{print $4}'`

	#计算
	UsedPerc=`free |grep "Mem"|awk '{printf("%3.2f%%",(($3/$2)*100))}'`
	FrePerc=`free |grep "Mem"|awk '{printf("%3.2f%%",(($4/$2)*100))}'`

	echo "total:$GetTotal_G used:$GetUsed_G($UsedPerc) free:$GetFree_G($FrePerc)"

}

#network
SYS_GetNetworkInfo()
{
	echo ""
	echo net:
	GetNetIP=(`ifconfig|grep -w "inet"|grep -vE 'inet6|127.0.0.1'|awk '{print $2}'`)
	GetNetInterface=(`ifconfig|grep ^[a-z]|awk '{print $1}'|sed 's/://g'`)

	for ((netIndex=0;netIndex<${#GetNetIP[@]};netIndex++))
	do
		#获取第一次上行
		GetTransBytes_1=`cat /proc/net/dev |grep -w ${GetNetInterface[$netIndex]}|awk '{print $10}'`
		#获取第一次下行
		GetRecvBytes_1=`cat /proc/net/dev |grep -w ${GetNetInterface[$netIndex]}|awk '{print $2}'`

		if [ $GetTransBytes_1 -ne 0 -a $GetTransBytes_1 -ne 0  ];then
			sleep 1
		fi
		#获取第二次上行
		GetTransBytes_2=`cat /proc/net/dev |grep -w ${GetNetInterface[$netIndex]}|awk '{print $10}'`
		#获取第二次下行
		GetRecvBytes_2=`cat /proc/net/dev |grep -w ${GetNetInterface[$netIndex]}|awk '{print $2}'`
		echo "interface:${GetNetInterface[$netIndex]} ip:${GetNetIP[$netIndex]} up:$(( $GetTransBytes_2-$GetTransBytes_1))Byte/s down:$(( $GetRecvBytes_2-$GetRecvBytes_1 ))Byte/s "
	done

}

#disk
SYS_GetDiskInfo()
{
	echo ""	
	echo "disk:"
	GetDiskDevice=(`df -h|sed 1d|awk '{print $1}'`)
	GetDiskTotal=(`df -h|sed 1d|awk '{print $2}'`)
	GetDiskUsed=(`df -h|sed 1d|awk '{print $3}'`)
	GetDiskAvial=(`df -h|sed 1d|awk '{print $4}'`)
	GetDiskPerc=(`df -h|sed 1d|awk '{print $5}'`)
	GetDiskMount=(`df -h|sed 1d|awk '{print $6}'`)
	echo "Get ${#GetDiskDevice[@]}device para:$#"

	bFindDevice=0
	for ((DiskIndex=0;DiskIndex<${#GetDiskDevice[@]};DiskIndex++))
	do
		#查询指定设备 
		if [ $# -gt 0 ];then

			#有对应的设备
			if [ $1 == ${GetDiskDevice[$DiskIndex]}  ];then

				echo "name:${GetDiskDevice[$DiskIndex]} Perc:${GetDiskPerc[$DiskIndex]} size:${GetDiskTotal[$DiskIndex]} Used:${GetDiskUsed[DiskIndex]}		avail:${GetDiskAvial[$DiskIndex]} perc:${GetDiskPerc[$DiskIndex]} mountd on:${GetDiskMount[$DiskIndex]} "
				let bFindDevice=1

			fi

		#无输入参数 全部输出
		elif [ $# -eq 0 ];then

			echo "name:${GetDiskDevice[$DiskIndex]} Perc:${GetDiskPerc[$DiskIndex]} size:${GetDiskTotal[$DiskIndex]} Used:${GetDiskUsed[DiskIndex]}	avail:${GetDiskAvial[$DiskIndex]} perc:${GetDiskPerc[$DiskIndex]} mountd on:${GetDiskMount[$DiskIndex]}"

		fi
	done

	#未找到指定设备
	if [ $bFindDevice -eq 0 -a $# -gt 0 ];then

			echo -e "\033[31mSYS_GetDiskInfo:[$1] does not exist\033[0m"
	fi
}

SYS_Help()
{
	echo "Usage:"
	echo -e "\033[32m $0 [options]\033[0m"
	echo ""
	echo "Options:"
	echo " -a, --all	read all information"
	echo " -o, --once	read only once"
	echo " -c, --cpu	read cpu information"
	echo " -m, --mem	read mem information"
	echo " -n, --network	read network information"
	echo " -d, --disk	read disk information"
	echo " -t, --time	read time information"
	echo ""
	exit
}

SYS_InputError()
{
	echo ""
	if [ $# -eq 1 ];then
		echo -e "\033[31mSYS_InputError:[$1] Is not a specified parameter\033[0m"
	else
		echo -e "\033[31mSYS_InputError\033[0m"

	fi
	echo "Usage:"
	echo -e "\033[32m $0 [options]\033[0m"
	echo ""
	echo "Options:"
	echo " -a, --all	read all information"
	echo " -o, --once	read only once"
	echo " -c, --cpu	read cpu information"
	echo " -m, --mem	read mem information"
	echo " -n, --network	read network information"
	echo " -d, --disk	read disk information"
	echo " -t, --time	read time information"
	echo ""
	exit
}

##############################变量定义############################## 

##############################脚本起始############################## 

#参数小于1 打印错误 并退出
if [ $# -lt 1 ];then
	SYS_InputError 
	exit
fi

#将传递的参数写入数组
sysArg=($@)
sysArgSize=$#

#判断输入参数是否有帮助
for((i=0;i<$sysArgSize;i++))
do 
	if [ ${sysArg[$i]:0:1} == "-" ];then

		if [ ${sysArg[$i]} == "-h" -o ${sysArg[$i]} == "--help" ];then
			SYS_Help
		elif [  ${sysArg[$i]} == "-a" -o ${sysArg[$i]} == "--all"  ];then
			continue
		elif [  ${sysArg[$i]} == "-o" -o ${sysArg[$i]} == "--once"  ];then
			continue
		elif [  ${sysArg[$i]} == "-c" -o ${sysArg[$i]} == "--cpu"  ];then
			continue
		elif [  ${sysArg[$i]} == "-m" -o ${sysArg[$i]} == "--mem"  ];then
			continue
		elif [  ${sysArg[$i]} == "-n" -o ${sysArg[$i]} == "--network"  ];then
			continue
		elif [  ${sysArg[$i]} == "-d" -o ${sysArg[$i]} == "--disk"  ];then
			continue
		elif [  ${sysArg[$i]} == "-t" -o ${sysArg[$i]} == "--time"  ];then
			continue
		else
			SYS_InputError ${sysArg[$i]}
		fi
	fi
done



#判断输入参数
for((i=0;i<$sysArgSize;i++))
do
	echo ""
	arg=${sysArg[$i]}
	echo -e "\033[32marg[$i]:$arg\033[0m"

	if [ $arg == "-a" -o $arg == "--all" ];then

		SYS_GetOnceInfo
		SYS_GetCpuInfo
		SYS_GetMemInfo
		SYS_GetNetworkInfo
		SYS_GetDiskInfo
		SYS_GetUptime
		exit

	elif [ $arg == "-o" -o $arg == "--once" ];then
		SYS_GetOnceInfo

	elif [ $arg == "-c" -o $arg == "--cpu" ];then
		SYS_GetCpuInfo

	elif [ $arg == "-m" -o $arg == "--mem" ];then
		SYS_GetMemInfo

	elif [ $arg == "-n" -o $arg == "--network" ];then
		SYS_GetNetworkInfo

	elif [ $arg == "-d" -o $arg == "--disk" ];then
		let i+=1
		#判断是否末尾
		if [ $i -eq $sysArgSize ];then
			#查询全部设备
			SYS_GetDiskInfo
		fi

		#用于判断是否有设备名的参数
		bHavePara=0
		for((j=$i;j<$sysArgSize;j++))
		do
			DiskArg=${sysArg[$j]}
			#参数为-开头 且 查找过设备
			if [ ${DiskArg:0:1} == "-" -a $bHavePara -gt 0 ];then
				let i=$j-1
				break
			#参数为-开头 且 未查找过设备
			elif [ ${DiskArg:0:1} == "-" -a $bHavePara -eq 0 ];then
				#查询全部设备
				SYS_GetDiskInfo
				break;
			fi

			SYS_GetDiskInfo $DiskArg 
			let bHavePara=1

			if [ $j -eq $((${sysArgSize}-1))  ];then
				exit
			fi
		done

	elif [ $arg == "-t" -o $arg == "--time" ];then
		SYS_GetUptime

	else
		SYS_InputError $arg
	fi

done

exit
