#ifndef TREE_H_
#define TREE_H_
#include <type_traits>
#include <vector>
namespace kTree
{
    struct BSTNodeImple{};
    struct AVLNodeImple : public BSTNodeImple{};

    template<typename T, typename NodeType>
    struct Node
    {
        using Type = NodeType;
        T* m_data{ nullptr };
        Node<T, NodeType>* parent{ nullptr };
        Node<T, NodeType>* leftNode{ nullptr };
        Node<T, NodeType>* rightNode{ nullptr };
    };
      
    template<typename T>
    struct Node<T, AVLNodeImple>       
    {
        using Type = AVLNodeImple;
        T* m_data{ nullptr };
        Node<T, AVLNodeImple>* parent{ nullptr };
        Node<T, AVLNodeImple>* leftNode{ nullptr };
        Node<T, AVLNodeImple>* rightNode{ nullptr };
        uint64_t m_height{ 0 };
    };

    template<typename T, typename = std::enable_if_t<!std::is_pointer_v<T>>>
    using BSTNode = Node<T, BSTNodeImple>;

    template<typename T, typename = std::enable_if_t<!std::is_pointer_v<T>>>
    using AVLNode = Node<T, AVLNodeImple>;

    template<typename T, typename NodeType = BSTNodeImple, typename = std::enable_if_t<!std::is_pointer_v<T>>>
    class BST{
    public:
        BST() = default;

        ~BST() {
            for (auto node : m_nodeCache) {
                delete node;
                node = nullptr;
            }
        }

        size_t Size(){
            return m_size;
        }
        
        template<typename U, 
        std::enable_if_t<std::is_convertible_v<std::decay_t<T>, std::decay_t<U>> && !std::is_pointer_v<U>>* = nullptr>
        Node<T, NodeType>* Insert(const U& value){
            using OriginalType = std::decay_t<T>;
            Node<T, NodeType>* tempPtr = new Node<OriginalType, NodeType>;
            tempPtr->m_data = new T(value);
            
            m_nodeCache.push_back(tempPtr);

            return InsertImple(value, tempPtr);
        }

        template<typename U, typename = std::enable_if_t<std::is_convertible_v<std::decay_t<U>, std::decay_t<T>>>>
        Node<T, NodeType>* Search(const U& value) {
            Node<T, NodeType>* search = m_rootNode;
            while (search != nullptr) {
                if (value < *search->m_data) {
                    search = search->leftNode;
                }
                else if(value > *search->m_data) {
                    search = search->rightNode;
                }
                else {
                    return search;
                }
            }
            return nullptr;
        }

        template<typename U, typename = std::enable_if_t<std::is_convertible_v<std::decay_t<U>, std::decay_t<T>>>>
        bool Remove(const U& value) {
            auto needRemoveNode = Search(value);
            if (needRemoveNode == nullptr) {
                return false;
            }
            //handle single node
            if (needRemoveNode->leftNode == nullptr) {
               RemoveSingleNodeImple(needRemoveNode, needRemoveNode->rightNode);
            }
            else if (needRemoveNode->rightNode == nullptr) {
                RemoveSingleNodeImple(needRemoveNode, needRemoveNode->leftNode);
            }
            else {
                //handle double node
                RemoveDoubleNodeImple(needRemoveNode);
            }
            m_size--;

            RemoveNodeInCache(needRemoveNode);

            delete needRemoveNode;
            return true;
        }

    protected:
        Node<T, NodeType>* m_rootNode{ nullptr };
        size_t m_size{0};

        //used to delete
        std::vector<Node<T, NodeType>*> m_nodeCache;

    private:

        template<typename U>
        Node<T, NodeType>* InsertImple(const U& value, Node<T, NodeType>* tempPtr){
            if(m_rootNode == nullptr){
                m_rootNode = tempPtr;
                m_size++;
                return m_rootNode;
            }
            Node<T, NodeType>* parent = m_rootNode;
            Node<T, NodeType>* search = m_rootNode;
            while(search != nullptr && search->m_data != nullptr){
                parent = search;
                if(value < *search->m_data){
                    search = search->leftNode;
                }
                else{
                    search = search->rightNode;
                }
            }

            tempPtr->parent = parent;
            if(value < *parent->m_data){
                parent->leftNode = tempPtr;
            }
            else{
                parent->rightNode = tempPtr;
            }
            m_size++;
            return tempPtr;
        }

        void RemoveSingleNodeImple(Node<T, NodeType>* willRemoveNode, Node<T, NodeType>* willReplaceNode) {
            auto fatherNode = willRemoveNode->parent;
            bool isLeft = *willRemoveNode->m_data < *fatherNode->m_data;
            if (isLeft) {
                fatherNode->leftNode = willReplaceNode;
            }
            else {
                fatherNode->rightNode = willReplaceNode;
            }
            if (willReplaceNode != nullptr) {
                willReplaceNode->parent = fatherNode;
            }
        }

        void RemoveDoubleNodeImple(Node<T, NodeType>* needRemoveNode) {
            size_t leftNodeDepth = 0;
            size_t rightNodeDepth = 0;

            auto searchLeftNode = needRemoveNode->leftNode;
            while (searchLeftNode->rightNode != nullptr) {
                searchLeftNode = searchLeftNode->rightNode;
                leftNodeDepth++;
            }

            auto searchRightNode = needRemoveNode->rightNode;
            while (searchRightNode->leftNode != nullptr) {
                searchRightNode = searchRightNode->leftNode;
                rightNodeDepth++;
            }

            auto fatherNode = needRemoveNode->parent;
            bool isLeft = *needRemoveNode->m_data < *fatherNode->m_data;
            if (leftNodeDepth > rightNodeDepth) {
                searchLeftNode->leftNode->parent = searchLeftNode->parent;
                searchLeftNode->parent->rightNode = searchLeftNode->leftNode;
                RemoveDoubleNodeImpleImple(searchLeftNode, fatherNode, needRemoveNode, isLeft);
            }
            else {
                searchRightNode->rightNode->parent = searchRightNode->parent;
                searchRightNode->parent->leftNode = searchRightNode->rightNode;
                RemoveDoubleNodeImpleImple(searchRightNode, fatherNode, needRemoveNode, isLeft);
            }
        }

        //swap some node
        void RemoveDoubleNodeImpleImple(Node<T, NodeType>*& willSwapNode, Node<T, NodeType>*& fatherNode, Node<T, NodeType>*& needRemoveNode, bool isLeft) {
            willSwapNode->parent = fatherNode;
            willSwapNode->leftNode = needRemoveNode->leftNode;
            needRemoveNode->leftNode->parent = willSwapNode;
            willSwapNode->rightNode = needRemoveNode->rightNode;
            needRemoveNode->rightNode->parent = willSwapNode;

            if (isLeft) {
                fatherNode->leftNode = willSwapNode;
            }
            else {
                fatherNode->rightNode = willSwapNode;
            }
        }
        
        void RemoveNodeInCache(Node<T, NodeType>* needRemoveNode) {
            auto it = std::find(m_nodeCache.begin(), m_nodeCache.end(), needRemoveNode);
            std::swap(*it, m_nodeCache.back());
            m_nodeCache.pop_back();
        }
    };


    template<typename T, typename NodeType = AVLNodeImple, typename = std::enable_if_t<!std::is_pointer_v<T>>>
    class AVL : public BST<T, NodeType>{
    public:

        template<typename U, 
        std::enable_if_t<!std::is_pointer_v<U> && std::is_convertible_v<std::decay_t<U>, std::decay_t<T>>>* = nullptr>
        AVLNode<T>* InsertAndBalance(const U& value){
            AVLNode<T>* returnPtr = nullptr;
            BST<T, NodeType>::Insert(value);

            //update 
            return returnPtr;
        }

        void TestRotateLeft(AVLNode<T>*& node) {
            RotateLeft(node);
        } 
        void TestRotateRight(AVLNode<T>*& node) {
            RotateRight(node);
        }
    private:

        void Refresh() {
            //clearUp warning be used this pointer
            this->m_rootNode->m_height++;
        }

        AVLNode<T>* RotateLeft(AVLNode<T>*& node) {
            AVLNode<T>* fatherNode = node->parent;
            AVLNode<T>* rotateNode = node->rightNode;
            rotateNode->parent = fatherNode;

            node->rightNode = rotateNode->leftNode;
            node->rightNode->parent = node;

            node->parent = rotateNode;
            rotateNode->leftNode = node;

            if (*node->m_data < *fatherNode->m_data) {
                fatherNode->leftNode = rotateNode;
            }
            else {
                fatherNode->rightNode = rotateNode;
            }
            return rotateNode;
        }

        AVLNode<T>* RotateRight(AVLNode<T>*& node) {
            AVLNode<T>* fatherNode = node->parent;
            AVLNode<T>* rotateNode = node->leftNode;
            rotateNode->parent = fatherNode;

            node->leftNode = rotateNode->rightNode;
            node->leftNode->parent = node;

            node->parent = rotateNode;
            rotateNode->rightNode = node;

            if (*node->m_data < *fatherNode->m_data) {
                fatherNode->leftNode = rotateNode;
            }
            else {
                fatherNode->rightNode = rotateNode;
            }
            return rotateNode;
        }

    };
} // namespace kTree
#endif