import json
import os
from shutil import copyfile

def createCorrectDataset(jsonLocation, baseImageLocation, baseNewDataset):
    with open(jsonLocation,'r') as jsonFile:
        dataset = json.load(jsonFile)
        for _, value in dataset.iteritems():
            for box in value:
                category = box["category"]
                filename = box["filename"]
                categoryDirectory = baseNewDataset+category+"/"
                if not os.path.exists(categoryDirectory):
                    os.makedirs(categoryDirectory)
                copyfile(baseImageLocation+filename, categoryDirectory+filename)

createCorrectDataset("/home/calvarez/outdoorImages.json", "/home/calvarez/Datasets/Street2Shop/Images/", "/home/calvarez/ClothinParsing/")