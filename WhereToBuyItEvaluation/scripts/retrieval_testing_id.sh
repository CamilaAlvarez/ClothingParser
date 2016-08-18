FILE=$1
if [ -f "retrieval.txt" ]
then
	rm "retrieval.txt"
fi

if [ -f "testing.txt" ]
then
	rm "testing.txt"
fi

while read line
do
	OUTPUT=$(echo $line |  awk -F '/' '{print $5}')
	CLASS=$(echo $line | awk -F '/' '{print $6}')
	ID=$(echo $line | awk '{print $1}')
	PRODUCT=$(echo $line | awk -F '?' '{print $2}')
	echo $line
	LINE="$ID\t$CLASS\t$PRODUCT"
	if [ $OUTPUT = "retrieval" ]
	then
		echo -e $LINE >> "retrieval.txt"
	elif [ $OUTPUT = "testing" ]
	then
		echo -e $LINE >> "testing.txt"
	fi

done<$FILE
