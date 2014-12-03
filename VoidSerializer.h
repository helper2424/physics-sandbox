#pragma once

#include <stdint.h>

class VoidSerializer {
public:

	enum SerializeClass : uint8_t {
		SERIALIZE_CLASS_BALL,
		SERIALIZE_CLASS_GATE,
		SERIALIZE_CLASS_CLIENT
	};

	VoidSerializer(void *pointer, SerializeClass);

	template<class T> T* get_pointer() const 
	{
		return static_cast<T*> (this->pointer);
	}
	
	SerializeClass get_serialize_class() const;
private:
	void *pointer;
	SerializeClass serialize_class;
};
