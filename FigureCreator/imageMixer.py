import cv2
import os
import numpy as np

#new_height = 400
def addRectangle(image, origin, finalPoint, output ):
    image = cv2.imread(image, cv2.CV_LOAD_IMAGE_COLOR)
    cv2.rectangle(image, origin, finalPoint, color=(0,255,153) ,thickness=10)
    cv2.imwrite(output, image)

def joinImages(dir, output):
    imagesNames = os.listdir(dir)
    total_width = 0
    images = []
    new_height = 0
    new_width = 0
    for image in imagesNames:
        if image == '.DS_Store':
            continue
        print image
        im = cv2.imread(dir+image, cv2.CV_LOAD_IMAGE_UNCHANGED)
        height, width = im.shape[:2]
        new_width = max(width, width)
        new_height += height
    for image in imagesNames:
        if image == '.DS_Store':
            continue
        print image
        im = cv2.imread(dir + image, cv2.CV_LOAD_IMAGE_UNCHANGED)
        height, width = im.shape[:2]
        im = cv2.resize(im,(new_width, height))
        #total_width += new_width
        images.append(im)

    print total_width
    blank_image = np.zeros((new_height, new_width, 3), np.uint8)
    i = 0
    for im in images:
        print im
        height , _ = im.shape[:2]
        blank_image[i:i+height,:, : ] = im
        i += height
    print i
    cv2.imwrite(output, blank_image)


if __name__ == '__main__':
    image = '/Users/calvarez/bags/query.jpg'
    #{"width": 737, "top": 1194, "left": 916, "height": 747}
    #tops : {"width": 986, "top": 693, "left": 693, "height": 1120}
    #eyewear: {"width": 415, "top": 413, "height": 121, "left": 677}
    #leggings: {"width": 698, "top": 858, "left": 68, "height": 766}
    #pants : {"width": 212, "top": 465, "left": 240, "height": 316}

    #addRectangle(image, (126, 47), (126 + 357, 47 + 383), '/Users/calvarez/exampleQuery/exampleDresses2.jpg')
    #addRectangle(image, (693, 693), (693 + 986, 693 + 1120), '/Users/calvarez/exampleQuery/exampleTops2.jpg')
    #addRectangle(image, (677, 413), (677 + 415, 413 + 121), '/Users/calvarez/exampleQuery/exampleEyewear2.jpg')
    #addRectangle(image, (68, 858), (68 + 698, 858 + 766), '/Users/calvarez/exampleQuery/exampleLeggings2.jpg')
    #addRectangle(image, (240, 465), (240 + 212, 465 + 316), '/Users/calvarez/exampleQuery/examplePants2.jpg')

    #addRectangle(image, (110, 447), (110 + 105, 447 + 231), '/Users/calvarez/bags/query-rectangle.jpg')
    joinImages('/Users/calvarez/chosenOuterwear/Group3/', '/Users/calvarez/outerwear.jpg')