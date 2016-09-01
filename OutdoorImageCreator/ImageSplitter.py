import json
import sys


def generateSplittedImageFile(output, inputFile):
    with open(inputFile) as jsonFile:
        imageJson = json.load(jsonFile)

        with open(output, 'w') as outputFile:
            for key, value in imageJson.iteritems():
                for element in value:
                    category = element["category"]
                    image = element["filename"]
                    outputFile.write(image+"\t"+category+"\n")

if __name__ == '__main__':
    args = sys.argv
    input = args[1]
    output = args[2]

    generateSplittedImageFile(output, input)