//
// Created by Camila Alvarez on 01-02-17.
//

#ifndef SIAMESEDATACROPPER_BOUNDINGBOX_HPP
#define SIAMESEDATACROPPER_BOUNDINGBOX_HPP


class BoundingBox {
private:
    int x;
    int y;
    int width;
    int height;
    int isActive;

public:
    BoundingBox(int x, int y, int width, int height, int isActive ):x(x),y(y),width(width),height(height),
    isActive(isActive){}
    inline int getX(){ return x; }
    inline int getY(){ return y; }
    inline int getWidth(){ return width; }
    inline int getHeight(){ return height; }
    inline int getIsActive(){ return isActive; }
};


#endif //SIAMESEDATACROPPER_BOUNDINGBOX_HPP
