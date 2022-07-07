#!/bin/bash

if [ $# -ne 4 ];then
	exit
fi

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

if [ $3 == 'cpu' ];then

	if [ $4 == 'a' ];then
		# 全部输出
		echo "{\"cpu\":\"all\"}"
		./client $1 $2 "{\"cpu\":\"all\"}"

		# 指定全部输出
		# echo "{\"cpu\":[{\"s\":[{\"jiffies\":\"all\"}]}]}"
		# ./client $1 $2 "{\"cpu\":[{\"s\":[{\"jiffies\":\"all\"}]}]}"
		# echo "{\"cpu\":[{\"s\":[{\"jiffies\":[{\"attr\":\"all\"}]}]}]}"
		# ./client $1 $2 "{\"cpu\":[{\"s\":[{\"jiffies\":[{\"attr\":\"all\"}]}]}]}"
		# echo "{\"cpu\":[{\"s\":[{\"jiffies\":[{\"name\":\"all\"}]}]}]}"
		# ./client $1 $2 "{\"cpu\":[{\"s\":[{\"jiffies\":[{\"name\":\"all\"}]}]}]}"
		# echo "{\"cpu\":[{\"s\":[{\"jiffies\":[{\"name\":\"all\"},{\"attr\":\"all\"}]}]}]}"
		# ./client $1 $2 "{\"cpu\":[{\"s\":[{\"jiffies\":[{\"name\":\"all\"},{\"attr\":\"all\"}]}]}]}"


		#除外全部输出
		# echo "{\"cpu\":[{\"e\":[\"wa\",\"id\"]},{\"jiffies\":\"all\"}]}"
		# ./client $1 $2 "{\"cpu\":[{\"e\":[\"wa\",\"id\"]},{\"jiffies\":\"all\"}]}"
		# echo "{\"cpu\":[{\"e\":[\"wa\",\"id\"]},{\"jiffies\":[{\"attr\":\"all\"}]}]}"
		# ./client $1 $2 "{\"cpu\":[{\"e\":[\"wa\",\"id\"]},{\"jiffies\":[{\"attr\":\"all\"}]}]}"
		# echo "{\"cpu\":[{\"e\":[\"wa\",\"id\"]},{\"jiffies\":[{\"name\":\"all\"}]}]}"
		# ./client $1 $2 "{\"cpu\":[{\"e\":[\"wa\",\"id\"]},{\"jiffies\":[{\"name\":\"all\"}]}]}"
		# echo "{\"cpu\":[{\"e\":[\"wa\",\"id\"]},{\"jiffies\":[{\"name\":\"all\"},{\"attr\":\"all\"}]}]}"
		# ./client $1 $2 "{\"cpu\":[{\"e\":[\"wa\",\"id\"]},{\"jiffies\":[{\"name\":\"all\"},{\"attr\":\"all\"}]}]}"
	
	
	elif [ $4 -eq 1 ];then
		# 指定显示 指定cpu 指定属性
		echo "{\"cpu\":[{\"s\":[\"wa\",\"id\",{\"jiffies\":[{\"name\":{\"s\":[\"cpu\"]}},{\"attr\":{\"s\":[\"name\",\"sys\",\"user\",\"nice\"]}}]}]}]}"
		./client $1 $2 "{\"cpu\":[{\"s\":[\"wa\",\"id\",{\"jiffies\":[{\"name\":{\"s\":[\"cpu\"]}},{\"attr\":{\"s\":[\"name\",\"sys\",\"user\",\"nice\"]}}]}]}]}"
	

	elif [ $4 -eq 2 ];then
		# 指定显示 除外cpu 指定属性
		echo "{\"cpu\":[{\"s\":[\"wa\",\"id\",{\"jiffies\":[{\"name\":{\"e\":[\"cpu0\"]}},{\"attr\":{\"s\":[\"name\",\"sys\",\"user\",\"nice\"]}}]}]}]}"
		./client $1 $2 "{\"cpu\":[{\"s\":[\"wa\",\"id\",{\"jiffies\":[{\"name\":{\"e\":[\"cpu0\"]}},{\"attr\":{\"s\":[\"name\",\"sys\",\"user\",\"nice\"]}}]}]}]}"
	

	elif [ $4 -eq 3 ];then
		# 指定显示  指定cpu  除外属性
		echo "{\"cpu\":[{\"s\":[\"wa\",\"id\",{\"jiffies\":[{\"name\":{\"s\":[\"cpu0\"]}},{\"attr\":{\"e\":[\"name\",\"sys\",\"user\",\"nice\"]}}]}]}]}"
		./client $1 $2 "{\"cpu\":[{\"s\":[\"wa\",\"id\",{\"jiffies\":[{\"name\":{\"s\":[\"cpu0\"]}},{\"attr\":{\"e\":[\"sys\",\"user\",\"nice\"]}}]}]}]}"
	

	elif [ $4 -eq 4 ];then
		# 指定显示  除外cpu  除外属性
		echo "{\"cpu\":[{\"s\":[\"wa\",\"id\",{\"jiffies\":[{\"name\":{\"e\":[\"cpu0\"]}},{\"attr\":{\"e\":[\"name\",\"sys\",\"user\",\"nice\"]}}]}]}]}"
		./client $1 $2 "{\"cpu\":[{\"s\":[\"wa\",\"id\",{\"jiffies\":[{\"name\":{\"e\":[\"cpu0\"]}},{\"attr\":{\"e\":[\"sys\",\"user\",\"nice\"]}}]}]}]}"
	

	elif [ $4 -eq 5 ];then
		# 除外显示 指定cpu 指定属性
		echo "{\"cpu\":[{\"e\":[\"wa\",\"id\"]},{\"jiffies\":[{\"name\":{\"s\":[\"cpu\"]}},{\"attr\":{ \"s\":[\"name\",\"sys\",\"user\",\"nice\"]}}]}]}"
		./client $1 $2 "{\"cpu\":[{\"e\":[\"wa\",\"id\"]},{\"jiffies\":[{\"name\":{\"s\":[\"cpu\"]}},{\"attr\":{ \"s\":[\"name\",\"sys\",\"user\",\"nice\"]}}]}]}"
	

	elif [ $4 -eq 6 ];then
		# 除外显示 除外cpu 指定属性
		echo "{\"cpu\":[{\"e\":[\"wa\",\"id\"]},{\"jiffies\":[{\"name\":{\"e\":[\"cpu\"]}},{\"attr\":{ \"s\":[\"name\",\"sys\",\"user\",\"nice\"]}}]}]}"
		./client $1 $2 "{\"cpu\":[{\"e\":[\"wa\",\"id\"]},{\"jiffies\":[{\"name\":{\"e\":[\"cpu\"]}},{\"attr\":{ \"s\":[\"name\",\"sys\",\"user\",\"nice\"]}}]}]}"
	

	elif [ $4 -eq 7 ];then
		# 除外显示  指定cpu  除外属性
		echo "{\"cpu\":[{\"e\":[\"wa\",\"id\"]},{\"jiffies\":[{\"name\":{\"s\":[\"cpu\"]}},{\"attr\":{ \"e\":[\"sys\",\"user\",\"nice\"]}}]}]}"
		./client $1 $2 "{\"cpu\":[{\"e\":[\"wa\",\"id\"]},{\"jiffies\":[{\"name\":{\"s\":[\"cpu\"]}},{\"attr\":{ \"e\":[\"sys\",\"user\",\"nice\"]}}]}]}"
	

	elif [ $4 -eq 8 ];then
		# 除外显示  除外cpu  除外属性
		echo "{\"cpu\":[{\"e\":[\"wa\",\"id\"]},{\"jiffies\":[{\"name\":{\"e\":[\"cpu\"]}},{\"attr\":{ \"e\":[\"sys\",\"user\",\"nice\"]}}]}]}"
		./client $1 $2 "{\"cpu\":[{\"e\":[\"wa\",\"id\"]},{\"jiffies\":[{\"name\":{\"e\":[\"cpu\"]}},{\"attr\":{ \"e\":[\"sys\",\"user\",\"nice\"]}}]}]}"

	elif [ $4 -eq 9 ];then
		# 除外  除外cpu  除外属性
		echo "{\"cpu\":[{\"e\":[\"wa\",\"id\",\"jiffies\"]},{\"jiffies\":[{\"name\":{\"e\":[\"cpu\"]}},{\"attr\":{ \"e\":[\"sys\",\"user\",\"nice\"]}}]}]}"
		./client $1 $2 "{\"cpu\":[{\"e\":[\"wa\",\"id\",\"jiffies\"]},{\"jiffies\":[{\"name\":{\"e\":[\"cpu\"]}},{\"attr\":{ \"e\":[\"sys\",\"user\",\"nice\"]}}]}]}"
	fi
	
	# ./client $1 $2 "{\"cpu\":\"all\"}"
fi

if [ $3 == 'disk' ];then
	./client $1 $2 "{\"disk\":\"all\"}"
fi



# ./client 1 1 {\"once\":\"all\",\"time\":{\"e\":[\"allsec\"]}}							#除了allsec都显示
# ./client 1 1 {\"once\":\"all\",\"time\":{\"s\":[\"sec\",\"min\",\"hour\",\"day\"]}}		#只显示sec min hour day
