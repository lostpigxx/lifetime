# auto test bash
# bash test.sh [index type] [maxthread] [r/w]

index_type="$1"
result_path="./result/$1.txt"
thread_num="$2"
mode="$3"


echo $(date +%F%n%T) >> $result_path
    if [ $mode = "w" ]; then
        for ((i = 1; i <= thread_num; i = i * 2))
        do
            ./index_bench --num 1000000 --benchmarks writerand --index $index_type --keylen 16 --thread $i >> $result_path
        done

    elif [ $mode = "r" ]; then
        for ((i = 1; i <= thread_num; i = i * 2))
        do
            ./index_bench --num 1000000 --benchmarks readrand --index $index_type --keylen 16 --thread $i >> $result_path
        done

    fi

echo "            " >> $result_path
echo "            " >> $result_path
echo "            " >> $result_path

