BASE=/home/calvarez/CroppedStreet2Shop/
RETRIEVAL="${BASE}retrieval/"

if [ -f classes.txt ]
then 
	rm classes.txt
fi

for folder in `ls $RETRIEVAL`
do
	SIZE=$( ls $RETRIEVAL$folder | wc -l )
	echo -e "${folder}\t${SIZE}" >> classes.txt 	
done
