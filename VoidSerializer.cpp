#include "VoidSerializer.h"

VoidSerializer::VoidSerializer(void *pointer, SerializeClass serialize_class) : pointer(pointer), serialize_class(serialize_class) 
{
}

VoidSerializer::SerializeClass VoidSerializer::get_serialize_class() const 
{
        return this->serialize_class;
}

