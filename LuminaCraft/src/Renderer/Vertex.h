#pragma once


struct Vertex
{
    Vertex(uint8_t _posX, uint8_t _posY, uint8_t _posZ, uint8_t _texGridX, uint8_t _texGridY)
    {
        posX = _posX;
        posY = _posY;
        posZ = _posZ;

        texGridX = _texGridX;
        texGridY = _texGridY;
    }

    uint8_t posX;
    uint8_t posY;
    uint8_t posZ;
    uint8_t texGridX;
    uint8_t texGridY;
};