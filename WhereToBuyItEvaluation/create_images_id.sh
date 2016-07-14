FILE=$1
OUTPUT=$2
COUNT=0

if [[ -f $OUTPUT  ]]
then
	rm $OUTPUT
fi

while read line
do
	PHOTO=$(echo "$line" | awk '{print $2}')
	COUNT=$((COUNT + 1))
	echo -e "${COUNT}\t${PHOTO}" >> $OUTPUT	
done<$FILE
