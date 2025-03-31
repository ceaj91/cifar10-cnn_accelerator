#ifndef FLATTENLAYER_HPP_INCLUDED
#define FLATTENLAYER_HPP_INCLUDED

#include "common.hpp"

class FlattenLayer {
private:
    vector2D output;
public:
    vector2D forward_prop(vector4D input);
};

#endif // FLATTENLAYER_HPP_INCLUDED
