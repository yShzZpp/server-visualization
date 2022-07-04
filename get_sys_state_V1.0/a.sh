#!/bin/bash


if [ $3 == 'all' ];then
	./client $1 $2 "{\"once\":\"all\",\"time\":\"all\",\"mem\":\"all\",\"cpu\":\"all\",\"net\":\"all\",\"disk\":\"all\"}"
fi

if [ $3 == 'once' ];then
	./client $1 $2 "{\"once\":{\"s\":[\"version\",\"distributor\",\"cpu_num\",\"host_name\",\"user_name\"]}}"
	./client $1 $2 "{\"once\":{\"s\":[\"distributor\",\"host_name\",\"user_name\"]}}"
	./client $1 $2 "{\"once\":{\"e\":[\"version\",\"distributor\",\"cpu_num\",\"host_name\",\"user_name\"]}}"
	# ./client $1 $2 "{\"once\":\"all\"}"
fi



# ./client 1 1 {\"once\":\"all\",\"time\":{\"e\":[\"allsec\"]}}							#除了allsec都显示
# ./client 1 1 {\"once\":\"all\",\"time\":{\"s\":[\"sec\",\"min\",\"hour\",\"day\"]}}		#只显示sec min hour day
