#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <list>
#include <functional>
#include <string>
#define MAXSIZE 10000

namespace aisdi
{

/*static template<typename keytype> size_t hash(const keytype& key)
    {
            return (size_t)key % SIZE;
        }
static template<> size_t hash<std::string>(const std::string& key)
        {
            size_t n=0;
            for(auto i: key)
                n += (size_t)i;
            return n;
        }*/

template<typename keytype> class myhash
{
    public:
    size_t hash(const keytype& key) const
    {
        return (size_t)key % 20;
    }
};

template<> class myhash<std::string>
{
    public:
    size_t hash(const std::string& key) const
    {
        size_t n=0;
        for(auto i: key)
            n += (size_t)i;
        return n;
    }
};

template <typename KeyType, typename ValueType>
class HashMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

  HashMap()
  {
    hashTab = new std::list<value_type>[SIZE];
  }

  HashMap(std::initializer_list<value_type> list): HashMap()
  {
    for(auto i: list)
        hashTab[hashKey(i.first)].push_back(i);
  }

  HashMap(const HashMap& other): HashMap()
  {
    clear();
    for(auto i: other)
        hashTab[hashKey(i.first)].push_back(i);
  }

  HashMap(HashMap&& other): HashMap()
  {
    clear();
    delete[] hashTab;
    hashTab = std::move(other.hashTab);
    other.hashTab = new std::list<value_type>[SIZE];
  }

  ~HashMap()
  {
    clear();
    delete[] hashTab;
  }

  HashMap& operator=(const HashMap& other)
  {
    if(this!=&other)
    {
      clear();
      for (auto i: other)
        hashTab[hashKey(i.first)].push_back(i);
    }
    return *this;
  }

  HashMap& operator=(HashMap&& other)
  {
    if (this!=&other)
    {
      clear();
      delete[] hashTab;
      hashTab = std::move(other.hashTab);
      other.hashTab = new std::list<value_type>[SIZE];
    }
    return *this;
  }

  bool isEmpty() const
  {
    for(size_type i=0; i<SIZE; ++i)
    {
        if(!hashTab[i].empty())
            return false;
    }
    return true;
  }

  mapped_type& operator[](const key_type& key)
  {
    size_type n = hashKey(key);
    for(auto &i: hashTab[n])
    {
        if(i.first==key)
        {
            return i.second;
        }
    }
    hashTab[n].push_back(std::make_pair(key, mapped_type{}));
    auto &i = *(--(hashTab[n].end()));
    return i.second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    size_type n = hashKey(key);
    for(auto &i: hashTab[n])
    {
        if(i.first==key)
        {
            return i.second;
        }
    }
    throw std::out_of_range("key not in map");
  }

  mapped_type& valueOf(const key_type& key)
  {
    size_type n = hashKey(key);
    for(auto &i: hashTab[n])
    {
        if(i.first==key)
        {
            return i.second;
        }
    }
    throw std::out_of_range("key not in map");
  }

  const_iterator find(const key_type& key) const
  {
    size_type n = hashKey(key);
    for(auto i=hashTab[n].begin(); i!=hashTab[n].end(); ++i)
    {
        if((*i).first==key)
        {
            return ConstIterator(this, i, n);
        }
    }
    return cend();
  }

  iterator find(const key_type& key)
  {
    size_type n = hashKey(key);
    for(auto i=hashTab[n].begin(); i!=hashTab[n].end(); ++i)
    {
        if((*i).first==key)
        {
            return Iterator(this, i, n);
        }
    }
    return end();
  }

  void remove(const key_type& key)
  {
    if (isEmpty())
      throw std::out_of_range("trying to remove from empty map");
    size_type n = hashKey(key);
    for(auto i=hashTab[n].begin(); i!=hashTab[n].end(); ++i)
    {
        if ((*i).first==key)
        {
            hashTab[n].erase(i);
            return;
        }
    }
    throw std::out_of_range("key not in map");
  }

  void remove(const const_iterator& it)
  {
    if (it==end())
      throw std::out_of_range("trying to remove end");
    if(hashTab[it.getIndex()].empty())
      return;
    hashTab[(it.getIndex())].erase(it.getListiter());
  }

  size_type getSize() const
  {
    size_type n = 0;
    for (size_type i=0; i<SIZE; ++i)
    {
        n += hashTab[i].size();
    }
    return n;
  }

  bool operator==(const HashMap& other) const
  {
    if(getSize()!=other.getSize()) return false;
    for(auto i: *this)
    {
        auto iother = other.find(i.first);
        if (other.end() == iother) return false;
        if (i.second!=(*iother).second) return false;
    }
    /*for(auto i = begin(), ioth = other.begin() ; i!=end(); ++i, ++ioth)
    {
        if(i != ioth)
        {
            return false;
        }
    }*/
    return true;
  }

  bool operator!=(const HashMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    return cbegin();
  }

  iterator end()
  {
    return cend();
  }

  const_iterator cbegin() const
  {
    for(size_type i=0; i<SIZE; ++i)
    {
        if(!hashTab[i].empty())
        {
            return ConstIterator(this, hashTab[i].begin(), i);
        }
    }
    return cend();
  }

  const_iterator cend() const
  {
    return ConstIterator(this, hashTab[SIZE-1].end(), SIZE-1);
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }
private:

    std::list<value_type>* hashTab;
    static const size_type SIZE = MAXSIZE;
    myhash<key_type> helpme;

    void clear()
    {
        for(size_type i=0; i<SIZE; ++i)
        {
            hashTab[i].clear();
        }
    }

    size_type hashKey(const key_type& key) const
    {
        return helpme.hash(key);
        //return makehash(key) % SIZE;
        //return hash<key_type>(key);
    }

    /*template<typename keytype> class penis
    {
        size_type hash(const keytype& key) const
        {
            return (size_t)key % SIZE;
        }
    };

    template<> class penis<std::string>
    {
        size_type hash(const string& key) const
        {
            size_type n=0;
            for(auto i: key)
                n += (size_type)i;
            return n;
        }
    };*/

};


/*template<typename ValueType> class HashMap<std::string, ValueType>: HashMap<size_t, ValueType>
{
    size_t hashKey(const std::string& key) const
    {
        size_t n;
        for(auto i: key)
        {
            n += (size_t)i;
        }
        return n % HashMap<size_t, ValueType>::SIZE;
    }
};*/

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;
  using list_iter = typename std::list<value_type>::iterator;

  explicit ConstIterator(const HashMap* hm, list_iter li, size_type id): mapptr(hm), lit(li), index(id)
  {}

  ConstIterator(const ConstIterator& other): ConstIterator(other.mapptr, other.lit, other.index)
  {}

  ConstIterator& operator++()
  {
    if(*this == mapptr->end())
    {
        throw std::out_of_range("trying to increment end");
    }
    ++lit;
    if(lit==mapptr->hashTab[index].end())
    {
        while(++index<mapptr->SIZE)
        {
            if(!mapptr->hashTab[index].empty())
            {
                lit = mapptr->hashTab[index].begin();
                return *this;
            }
        }
        *this = mapptr->end();
    }
    return *this;
  }

  ConstIterator operator++(int)
  {
    ConstIterator temp = *this;
    operator++();
    return temp;
  }

  ConstIterator& operator--()
  {
    if(*this == mapptr->begin())
    {
        throw std::out_of_range("trying to decrement begin");
    }

    if(*this == mapptr->end())
    {
        do
        {
            if(!mapptr->hashTab[index].empty())
            {
                lit = --(mapptr->hashTab[index].end());
                return *this;
            }
        } while (--index>=0);
    }

    if(lit==mapptr->hashTab[index].begin())
    {
        while(--index>=0)
        {
            if(!mapptr->hashTab[index].empty())
            {
                lit = --(mapptr->hashTab[index].end());
                return *this;
            }
        }
    }
    --lit;
    return *this;
  }

  ConstIterator operator--(int)
  {
    ConstIterator temp = *this;
    operator--();
    return temp;
  }

  reference operator*() const
  {
    if (*this==mapptr->end())
      throw std::out_of_range("trying to dereference end");
    return *lit;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return mapptr == other.mapptr && lit == other.lit && index==other.index;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }

  size_type getIndex() const
  {
    return index;
  }

  list_iter getListiter() const
  {
    return lit;
  }

private:
    const HashMap* mapptr;
    list_iter lit;
    size_type index;
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;
  using list_iter = typename std::list<value_type>::iterator;

  explicit Iterator(const HashMap* hm, list_iter li, size_type id): ConstIterator(hm, li, id)
  {}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif /* AISDI_MAPS_HASHMAP_H */
