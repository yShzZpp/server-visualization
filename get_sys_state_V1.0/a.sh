#!/bin/bash


if [ $3 == 'all' ];then
	./client $1 $2 "{\"once\":\"all\",\"time\":\"all\",\"mem\":\"all\",\"cpu\":\"all\",\"net\":\"all\",\"disk\":\"all\"}"
fi

if [ $3 == 'once' ];then
	./client $1 $2 "{\"once\":{\"s\":[\"version\",\"distributor\",\"cpu_num\",\"host_name\",\"user_name\"]}}"
	./client $1 $2 "{\"once\":{\"s\":[\"distributor\",\"host_name\",\"user_name\"]}}"
	./client $1 $2 "{\"once\":{\"e\":[\"distributor\",\"cpu_num\",\"host_name\",\"user_name\"]}}"
	# ./client $1 $2 "{\"once\":\"all\"}"
fi

if [ $3 == 'time' ];then
	./client $1 $2 "{\"time\":{ \"s\":[\"allsec\",\"day\",\"hour\",\"min\",\"sec\"]}}"
	./client $1 $2 "{\"time\":{ \"e\":[\"day\",\"hour\",\"min\",\"sec\"]}}"
	./client $1 $2 "{\"time\":\"all\"}"
fi

if [ $3 == 'mem' ];then
	./client $1 $2 "{\"mem\":{ \"s\":[ \"total_mem\",\"used_mem\",\"free_mem\"] }}"
	./client $1 $2 "{\"mem\":{ \"e\":[ \"shared_mem\",\"buff_mem\",\"available_mem\" , \"total_low\", \"used_low\", \"free_low\" , \"total_high\",\"used_high\",\"free_high\", \"total_swap\",\"used_swap\",\"free_swap\"] }}"
	./client $1 $2 "{\"mem\":\"all\"}"
fi



# ./client 1 1 {\"once\":\"all\",\"time\":{\"e\":[\"allsec\"]}}							#除了allsec都显示
# ./client 1 1 {\"once\":\"all\",\"time\":{\"s\":[\"sec\",\"min\",\"hour\",\"day\"]}}		#只显示sec min hour day
