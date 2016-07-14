FOLDER=$1
FILE=$2

if [[ -f $FILE ]]
	then
	rm $FILE
fi

cd $FOLDER
for folder in `ls`
do
	if ! [[ -d $folder ]]
		then
		continue
	fi

	for image in `ls $folder`
	do
		IFS='?' read -ra ADDR <<< $image
		echo -e "${ADDR[-1]}\t${FOLDER}/${folder}/${image}\t${folder}">>$FILE
	done

done
