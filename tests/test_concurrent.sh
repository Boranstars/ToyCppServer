#!/bin/bash

# 并发请求数量
REQUESTS=5

# 服务器URL
URL="http://localhost:11451/"

# 循环创建多个背景curl进程
for ((i=1; i<=$REQUESTS; i++))
do
    # 使用curl访问服务器，并将输出重定向到文件（避免输出混乱）
    curl -s "$URL" > /dev/null 2>&1 &
    # 记录进程ID
    PIDS[$i]=$!
    echo "Started request $i (PID: ${PIDS[$i]})"
done

# 等待所有背景进程结束
for pid in ${PIDS[*]}
do
    wait $pid
done

echo "All requests completed."
