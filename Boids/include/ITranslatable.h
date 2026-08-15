#pragma once

template<typename T>
class ITranslatable
{
public:
	virtual const T& getTranslation() = 0;
	virtual void setTranslation(const T& translation) = 0;
};