#pragma once

template<typename T, typename V>
V& array_find(const T& array, const V& item)
{
	return std::find(array.begin(), array.end(), item);
}

template<typename T, typename V, typename Ret = typename T::value_type>
Ret array_find_nullable(const T& array, const V& item)
{
    auto result = std::find(array.begin(), array.end(), item);
    if (result != array.end())
        return *result;
    
    return nullptr;
}

template<typename T, typename V, typename Ret = typename T::value_type>
Ret array_findif_nullable(const T& array, const V& pred)
{	   
    auto result = std::find_if(array.begin(), array.end(), pred);
    if (result != array.end())
        return *result;

    return nullptr;
}

template<typename T, typename V>
bool array_erase(T& array, const V& item)
{
	auto result = std::find(array.begin(), array.end(), item);
    if (result != array.end())
    {
        array.erase(result);
        return true;
    }

	return false;
}

template<typename T, typename V>
bool array_eraseif(T& array, const V& pred)
{
    auto result = std::find_if(array.begin(), array.end(), pred);
    if (result != array.end())
    {
        array.erase(result);
        return true;
    }

    return false;
}

template<typename T, typename V>
bool array_exist(const T& array, const V& item)
{
	return std::find(array.begin(), array.end(), item) != array.end();
}

template<typename T, typename V>
bool array_existif(const T& array, const V& pred)
{
    return std::find_if(array.begin(), array.end(), pred) != array.end();
}

template<typename T, typename V>
void array_remove(T& array, const V& item)
{
	std::remove(array.begin(), array.end(), item);
}