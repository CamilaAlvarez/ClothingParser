import matplotlib.pyplot as plt
import numpy as np
from pylab import *

classes = ['bags', 'belts', 'dresses', 'eyewear', 'footwear', 'hats'
    ,'leggings', 'outerwear', 'pants', 'skirts', 'tops']
#base_dir = '/Users/calvarez/resultsByClass/'
base_dir = '/Users/calvarez/resultsByClass/'

def barChar(classesDictionary):
    N = len(classes)
    alexnetValues = []
    googlenetValues = []
    vgg16Values = []

    for category in classes:
        netsDictionary = classesDictionary[category]
        print netsDictionary
        alexnetValues.append(netsDictionary['AlexNet'])
        googlenetValues.append(netsDictionary['GoogleNet'])
        vgg16Values.append(netsDictionary['VGG-16'])

    pos = arange(11) + 0.8  # the bar centers on the y axis
    width = 0.2  # the width of the bars

    fig, ax = plt.subplots()
    alexnet = ax.barh(pos, alexnetValues, width, color='r')
    googlenet = ax.barh(pos + width, googlenetValues, width, color='y')
    vgg16 = ax.barh(pos + 2 * width, vgg16Values, width, color='b')
    #barh(pos, val, align='center')
    yticks(pos, classes, fontsize=30)
    grid(True)

    ind = np.arange(N)  # the x locations for the groups

    print len(alexnetValues)



    # add some text for labels, title and axes ticks
    ax.set_xlabel('top-100 retrieval accuracy', fontsize=30)
    ax.set_title('top-100 retrieval accuracy for the best layer of each architecture', size=30)
    #ax.set_xticks(ind + width)
    #ax.set_xticklabels(classes)

    ax.legend((alexnet[0], googlenet[0], vgg16[0]), ('AlexNet fc7', 'GoogleNet loss1/fc', 'VGG-16 fc6'), prop={'size':25})


    plt.show()

def getTop100FromNet(net, filename, classDictionary, layer, maxNetLayer):
    with open(filename, 'r') as classNetResults:
        lines = classNetResults.readlines()
        for line in lines:
            resultsPerClass = line.split('\t')

            if resultsPerClass[0] == 'STEP' or resultsPerClass[0] == 'average' or resultsPerClass[0] == '\n' or\
                len(resultsPerClass) == 1:
                continue
            result = resultsPerClass[12]
            print resultsPerClass[0] +" "+net+ " " + layer + " "+ result
            if net in classDictionary[resultsPerClass[0]]:
                previousResult = classDictionary[resultsPerClass[0]][net]
                if previousResult > result:
                    result = previousResult
                else:
                    maxNetLayer[net] = layer
            else:
                maxNetLayer[net] = layer
            classDictionary[resultsPerClass[0]][net] = result


def getFirst6Points(filename, values, net, layer):
    with open(filename, 'r') as classNetResults:
        lines = classNetResults.readlines()
        for line in lines:
            resultsPerClass = line.split('\t')

            if resultsPerClass[0] == 'STEP' or resultsPerClass[0] == '\n' or \
                            len(resultsPerClass) == 1:
                continue
            if resultsPerClass[0] == 'outerwear':
                values[net+' '+layer] = resultsPerClass[2:13]
                break



def showAllExperiments(resultsPerLayer):
    x = np.linspace(0,100, num=11)
    print resultsPerLayer['AlexNet fc6']
    #plt.plot(x, resultsPerLayer['AlexNet fc6'], '-^', linewidth=2.0, markersize=8)
    plt.plot(x, resultsPerLayer['AlexNet fc7'], '-*', linewidth=2.0, markersize=8)
    #plt.plot(x, resultsPerLayer['GoogleNet loss1/conv'], '->')
    plt.plot(x, resultsPerLayer['GoogleNet loss1/fc'],'-s', linewidth=2.0, markersize=8)
    #plt.plot(x, resultsPerLayer['GoogleNet loss2/fc'],'-D')
    plt.plot(x, resultsPerLayer['VGG-16 fc6'],'-x', linewidth=2.0, markersize=10)
    #plt.plot(x, resultsPerLayer['VGG-16 fc7'],'-o')
    plt.ylabel('Outerwear retrieval accuracy',fontsize=30)
    plt.title('Outerwear retrieval accuracy for each layer',fontsize=30)
    plt.xlabel('Retrieved items',fontsize=30)
    plt.xticks(x, [0,10,20,30,40,50,60,70,80,90,100])
    #plt.legend(['AlexNet fc6', 'AlexNet fc7', 'GoogleNet loss1/conv', 'GoogleNet loss1/fc',
    #             'GoogleNet loss2/fc','VGG-16 fc6', 'VGG-16 fc7'], loc='upper left',prop={'size':25})
    plt.legend(['AlexNet fc7', 'GoogleNet loss1/fc', 'VGG-16 fc6'], loc='upper left',prop={'size':25})
    print resultsPerLayer
    plt.show()

def getClassDictionary(mapFile):
    classDictionary = {}
    maxNetLayer = {}
    for category in classes:
        classDictionary[category] = {}

    for key, value in mapFile.iteritems():
        net = value['net']
        layer = value['layer']
        getTop100FromNet(net, base_dir + key, classDictionary, layer, maxNetLayer)

    print maxNetLayer
    return classDictionary


def getResultsPerLayer(mapFile):
    values = {}
    for key, value in mapFile.iteritems():
        net = value['net']
        layer = value['layer']
        getFirst6Points(base_dir+key, values, net, layer)

    showAllExperiments(values)

if __name__ == '__main__':

    mapFile = {
        'exactAccuracyVsRetrievedAlexNetFc6.txt' : {'net': 'AlexNet', 'layer': 'fc6'},
        'exactAccuracyVsRetrievedAlexNetFc7.txt': {'net': 'AlexNet', 'layer': 'fc7'},
        'exactAccuracyVsRetrievedGoogleNetLoss1Conv.txt': {'net': 'GoogleNet', 'layer': 'loss1/conv'},
        'exactAccuracyVsRetrievedGoogleNetLoss1Fc.txt': {'net': 'GoogleNet', 'layer': 'loss1/fc'},
        'exactAccuracyVsRetrievedGoogleNetLoss2Fc.txt': {'net': 'GoogleNet', 'layer': 'loss2/fc'},
        'exactAccuracyVsRetrievedVGGf6.txt': {'net': 'VGG-16', 'layer': 'fc6'},
        'exactAccuracyVsRetrievedVGGf7.txt': {'net': 'VGG-16', 'layer': 'fc7'},
    }

    mapFileBar = {
        'exactAccuracyVsRetrievedAlexNetFc7.txt': {'net': 'AlexNet', 'layer': 'fc7'},
        'exactAccuracyVsRetrievedGoogleNetLoss1Fc.txt': {'net': 'GoogleNet', 'layer': 'loss1/fc'},
        'exactAccuracyVsRetrievedVGGf6.txt': {'net': 'VGG-16', 'layer': 'fc6'}
    }

    barChar(getClassDictionary(mapFileBar))
    #
    #getResultsPerLayer(mapFile)

