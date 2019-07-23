#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <cstdlib>
#include<iostream>
#include<queue>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
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

  TreeMap()
  {
    root = new Node();
    root->right = root;
    csize = 0;
  }

  TreeMap(std::initializer_list<value_type> list): TreeMap()
  {
    for(auto element : list)
      operator[](element.first) = element.second;
  }

  TreeMap(const TreeMap& other): TreeMap()
  {
    for(auto element : other)
      operator[](element.first) = element.second;
  }

  TreeMap(TreeMap&& other)//: TreeMap()
  {
    //clear();
    //Node* temp = root;
    root = other.root;
    other.root = nullptr;
    csize = std::move(other.csize);
    other.csize = 0;
  }

  ~TreeMap()
  {
    if(root!=nullptr)
    {
        if(!isEmpty())
            clear();
        delete root;
    }
  }

  TreeMap& operator=(const TreeMap& other)
  {
    if(this == &other) return *this;
    clear();
    for(auto element : other)
      operator[](element.first) = element.second;
    return *this;
  }

  TreeMap& operator=(TreeMap&& other)
  {
    clear();
    Node* temp = root;
    root = other.root;
    other.root = temp;
    csize = std::move(other.csize);
    other.csize = 0;
    return *this;
  }

  bool isEmpty() const
  {
    return csize == 0;
  }

  mapped_type& operator[](const key_type& key)
  {
    if(isEmpty())
    {
        Node *node = new Node(root, key);
        root->left = node;
        ++csize;
        update(node);
        return node->data->second;
    }
    Node *temp = root->left;
    Node *cur = nullptr;
    while(temp!=nullptr)
    {
        cur = temp;
        if(key == cur->data->first)
        {
            return cur->data->second; //klucz juz wystapil
        }
        temp = key<cur->data->first? cur->left: cur->right;
    }
    temp = new Node(cur, key);
    if(key<cur->data->first)
    {
        cur->left = temp;
    }
    else cur->right = temp;
    ++csize;
    update(temp);
    updateParents(temp);
    return temp->data->second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    if(isEmpty())
    {
        throw std::out_of_range("tree empty");
    }
    const_iterator cpos = find(key);
    if(cpos == cend())
    {
        throw std::out_of_range("key not in tree");
    }
    return cpos->second;
  }

  mapped_type& valueOf(const key_type& key)
  {
    if(isEmpty())
    {
        throw std::out_of_range("tree empty");
    }
    iterator pos = find(key);
    if(pos == end())
    {
        throw std::out_of_range("key not in tree");
    }
    return pos->second;
  }

  const_iterator find(const key_type& key) const
  {
    if(isEmpty())
        return cend();
    return search(root->left, key);
  }

  iterator find(const key_type& key)
  {
    if(isEmpty())
        return end();
    return search(root->left, key);
  }

  void remove(const key_type& key)
  {
    if(isEmpty())
      throw std::out_of_range("trying to remove from empty tree");
    Node* toRemove = find(key).getCnode();
    if(toRemove == root)
        throw std::out_of_range("trying to remove element not in tree");
    Node* toBalance = toRemove->parent;
    if(toRemove->left==nullptr)
    {
        moveTree(toRemove, toRemove->right);
    }
    else if(toRemove->right==nullptr)
    {
        moveTree(toRemove, toRemove->left);
    }
    else
    {
        Node *temp = toRemove->right->getMinChild();
        toBalance = temp->parent;
        if(temp->parent!=toRemove)
        {
            moveTree(temp, temp->right);
            temp->right = toRemove->right;
            temp->right->parent = temp;
        }
        moveTree(toRemove, temp);
        temp->left = toRemove->left;
        temp->left->parent = temp;
    }
    delete toRemove;
    --csize;
    if(isEmpty())
    {
        root->left = nullptr;
    }
    else
    {
        //updateChildren(toBalance);
        if(toBalance!=root) updateParents(toBalance);
    }
  }

  void remove(const const_iterator& it)
  {
    if(it == end())
      throw std::out_of_range("trying to remove end");
    remove(it->first);
  }

  size_type getSize() const
  {
    return csize;
  }

  bool operator==(const TreeMap& other) const
  {
    if(csize!=other.getSize())
        return false;
    auto othIt = other.cbegin();
    auto ownIt = cbegin();
    for(size_type i=0; i<csize; ++i)
    {
        if(othIt->first!=ownIt->first || othIt->second!=ownIt->second)
            return false;
        ++othIt;
        ++ownIt;
    }
    return true;
  }

  bool operator!=(const TreeMap& other) const
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
    if(isEmpty()) return cend();
    return ConstIterator(root->getMinChild());
  }

  const_iterator cend() const
  {
    return ConstIterator(root);
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }

  void BFS()    //FUNKCJA POMOCNICZA DO WYPISYWANIA DRZEWA WARSTWAMI OD KORZENIA
  {
    int n, i=0;
    Node* temp = cend().getCnode()->left;
    std::queue<Node*> q;
    q.push(temp);
    while(!q.empty())
    {
        ++i;
        if(i==2 || i==4 || i==8 || i==16 || i==32 || i==64 || i==128 || i==256 || i==512) std::cout<<std::endl;
        if(temp==nullptr) n = 0;
        else n = temp->data->first;
        std::cout<<n<<" ";
        if(temp!=nullptr) q.push(temp->left);
        if(temp!=nullptr) q.push(temp->right);
        q.pop();
        temp = q.front();
    }
    //std::cin>>n;
  }

private:
  class Node
  {
    public:
      Node *left, *right, *parent;
      value_type *data;
      int height, balance;
      Node(): left(nullptr), right(nullptr), parent(nullptr), data(nullptr)
      {}
      Node(Node* p): Node()
      {
        parent = p;
      }
      Node(Node* p, const key_type& key): Node(p)
      {
        data = new value_type(key, mapped_type{});
      }
      Node(const key_type& key): Node(nullptr, key)
      {}
      ~Node()
      {
        if(data!=nullptr) delete data;
      }
      Node* getMinChild()
      {
        Node* node = this;
        while(node->left != nullptr)
        {
            node = node->left;
        }
        return node;
      }
  };
  Node *root;
  size_type csize;

  void swap(TreeMap& a, TreeMap& b)
  {
    std::swap(a.root, b.root);
    std::swap(a.csize, b.csize);
  }

  const_iterator search(Node* node, const key_type& key) const
  {
    while(node!=nullptr)
    {
        if(key==node->data->first)
        {
            return ConstIterator(node);
        }
        if(key<node->data->first)
        {
            node = node->left;
        }
        else
        {
            node = node->right;
        }
    }
    return cend();
  }

  void moveTree(Node *node1, Node *node2)
  {
    if(node1->parent == nullptr)
      root->left = node2;
    else if (node1 == node1->parent->left)
      node1->parent->left = node2;
    else
      node1->parent->right = node2;
    if(node2 != nullptr)
    {
        if(node2==node2->parent->left) node2->parent->left = nullptr;
        else node2->parent->right = nullptr;
        node2->parent = node1->parent;
    }
  }

  void clear()
  {
    clearSubtree(root->left);
    root->left = nullptr;
    csize = 0;
  }
  void clearSubtree(Node *node)
  {
    if(node==nullptr)return;
    if(node->left!=nullptr)
    {
        clearSubtree(node->left);
    }
    if(node->right!=nullptr)
    {
        clearSubtree(node->right);
    }
    --csize;
    delete node;
  }

  void update(Node* toUpdate)
  {
    toUpdate->height = 1;
    toUpdate->balance = 0;
    if(toUpdate->left!=nullptr)
    {
        toUpdate->height = toUpdate->left->height + 1;
        toUpdate->balance = toUpdate->left->height;
    }
    if(toUpdate->right!=nullptr)
    {
        toUpdate->height = std::max(toUpdate->right->height + 1, toUpdate->height);
        toUpdate->balance -= toUpdate->right->height;
    }
  }

  void updateParents(Node* toUpdate)
  {
    if(toUpdate->parent == nullptr || toUpdate == nullptr) return;
    /*toUpdate->children = toUpdate->balance = 0;
    if(toUpdate->left!=nullptr)
    {
        toUpdate->children = toUpdate->balance = toUpdate->left->children + 1;
    }
    if(toUpdate->right!=nullptr)
    {
        toUpdate->children += toUpdate->right->children + 1;
        toUpdate->balance -= toUpdate->right->children + 1;
    }
    if(std::abs(toUpdate->balance)>1) balance(toUpdate);*/
    update(toUpdate);
    if(std::abs(toUpdate->balance)>1) balance(toUpdate);
    updateParents(toUpdate->parent);
  }

  void RR(Node* toBalance)
  {
    Node* temp;
    temp = toBalance->right;
    toBalance->right = temp->left;
    if(temp->left!=nullptr) temp->left->parent = toBalance;
    temp->left = toBalance;
    temp->parent = toBalance->parent;
    if(toBalance->parent->left==toBalance)toBalance->parent->left = temp;
    else toBalance->parent->right = temp;
    toBalance->parent = temp;
    update(toBalance);
    update(temp);
  }

  void LL(Node* toBalance)
  {
    Node* temp;
    temp = toBalance->left;
    toBalance->left = temp->right;
    if(temp->right!=nullptr) temp->right->parent = toBalance;
    temp->right = toBalance;
    temp->parent = toBalance->parent;
    if(toBalance->parent->left==toBalance)toBalance->parent->left = temp;
    else toBalance->parent->right = temp;
    toBalance->parent = temp;
    update(toBalance);
    update(temp);
  }

  void balance(Node* toBalance)
  {
    if(toBalance->balance==-2)
    {
        if(toBalance->right->balance<1) //RR
        {
            RR(toBalance);
        }
        else //RL
        {
            LL(toBalance->right);
            RR(toBalance);
        }
    }
    else
    {
        if(toBalance->left->balance>-1) //LL
        {
            LL(toBalance);
        }
        else //LR
        {
            RR(toBalance->left);
            LL(toBalance);
        }
    }
    //BFS();
  }



};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{
private:
  Node *cnode;
public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type*;

  explicit ConstIterator(): cnode(nullptr)
  {}

  ConstIterator(Node* node): cnode(node)
  {}

  ConstIterator(const ConstIterator& other): cnode(other.cnode)
  {}

  ConstIterator& operator++()
  {
    if(cnode->parent==nullptr)
    {
        throw std::out_of_range("Trying to increment end");
    }
    if(cnode->right!=nullptr)
    {
        cnode = cnode->right->getMinChild();
        return *this;
    }
    Node* temp = cnode->parent;
    while(temp!=nullptr && cnode == temp->right)
    {
        cnode = temp;
        temp = temp->parent;
    }
    cnode = temp;
    return *this;
  }

  ConstIterator operator++(int)
  {
    ConstIterator i(*this);
    operator++();
    return i;
  }

  ConstIterator& operator--()
  {
    if(cnode->right == cnode)   //tylko w root, ustawiane w konstruktorze
    {
        if(cnode->left==nullptr)
        {
            throw std::out_of_range("Trying to decrement iterator to empty map");
        }
        cnode = cnode->left;
        while(cnode->right!=nullptr) cnode = cnode->right;
        return *this;
    }
    if(cnode->left!=nullptr)
    {
        cnode = cnode->left;
        while(cnode->right!=nullptr)
        {
            cnode = cnode->right;
        }
        return *this;
    }
    Node* temp = cnode->parent;
    while(temp!=nullptr && cnode==temp->left)
    {
        if(temp->parent==nullptr)
        {
            throw std::out_of_range("Trying to decrement begin");
        }
        cnode = temp;
        temp = temp->parent;
    }
    cnode = temp;
    return *this;
  }

  ConstIterator operator--(int)
  {
    ConstIterator i(*this);
    operator--();
    return i;
  }

  reference operator*() const
  {
    if(cnode->right == cnode)
      throw std::out_of_range("Trying to dereference end");
    return *(cnode->data);
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return cnode == other.cnode;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }

  Node* getCnode()
  {
    return cnode;
  }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;

  explicit Iterator()
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

#endif /* AISDI_MAPS_MAP_H */
