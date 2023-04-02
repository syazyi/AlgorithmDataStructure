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

        Node<T, NodeType>* GetRootNodePtr() {
            return m_rootNode;
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
            AVLNode<T>* returnPtr = BST<T, NodeType>::Insert(value);

            ComputeHeight(this->m_rootNode);
            Balance(returnPtr);
            return returnPtr;
        }

        void TestRotateLeft(AVLNode<T>*& node) {
            RotateLeft(node);
        } 

        void TestRotateRight(AVLNode<T>*& node) {
            RotateRight(node);
        }

        void RefreshHeight(AVLNode<T>* rootNode) {
            //clearUp warning be used this pointer
            ComputeHeight(rootNode);
        }
    private:

        uint64_t ComputeHeight(AVLNode<T>* rootNode) {
            if (rootNode == nullptr) {
                return 0;
            }
            if (rootNode->leftNode == nullptr && rootNode->rightNode == nullptr) {
                rootNode->m_height = 0;
                return 0;
            }
            auto leftHeight = ComputeHeight(rootNode->leftNode);
            auto rightHeight = ComputeHeight(rootNode->rightNode);
            rootNode->m_height = leftHeight > rightHeight ? leftHeight + 1 : rightHeight + 1;
            return rootNode->m_height;
        }

        AVLNode<T>* RotateLeft(AVLNode<T>*& node) {
            AVLNode<T>* fatherNode = node->parent;
            AVLNode<T>* rotateNode = node->rightNode;
            rotateNode->parent = fatherNode;

            node->rightNode = rotateNode->leftNode;
            node->rightNode->parent = node;

            node->parent = rotateNode;
            rotateNode->leftNode = node;

            if (fatherNode == nullptr) {
                return rotateNode;
            }
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

            if (fatherNode == nullptr) {
                return rotateNode;
            }
            if (*node->m_data < *fatherNode->m_data) {
                fatherNode->leftNode = rotateNode;
            }
            else {
                fatherNode->rightNode = rotateNode;
            }
            return rotateNode;
        }

        void Balance(AVLNode<T>* node) {
            while (node != nullptr) {
                auto parent = node->parent;

                auto difference = GetLeftRightDifference(node);
                switch (difference) {
                case 2:
                    if (GetLeftRightDifference(node) >= 0) {
                        RotateRight(node);
                        FindRootNode(node);
                        ComputeHeight(this->m_rootNode);
                    }
                    else {
                        RotateLeft(node->leftNode);
                        RotateRight(node);
                        FindRootNode(node);
                        ComputeHeight(this->m_rootNode);
                    }
                    break;
                case -2:
                    //���ڵ��� 
                    if (GetLeftRightDifference(node) >= 0) {
                        RotateLeft(node);
                        FindRootNode(node);
                        ComputeHeight(this->m_rootNode);
                    }
                    //���ڵ��Ҳ�����������
                    else {
                        RotateRight(node->rightNode);
                        RotateLeft(node);
                        FindRootNode(node);
                        ComputeHeight(this->m_rootNode);
                    }
                    break;
                default:
                    break;
                }
                node = parent;
            }
        }

        int64_t GetLeftRightDifference(const AVLNode<T>* node) const{
            int64_t leftHeight = (node->leftNode == nullptr) ? -1 : node->leftNode->m_height;
            int64_t rightHeight = (node->rightNode == nullptr) ? -1 : node->rightNode->m_height;

            return leftHeight - rightHeight;
        }

        AVLNode<T>* FindRootNode(AVLNode<T>* node) {
            if (node == nullptr) {
                return nullptr;
            }
            while (node->parent != nullptr) {
                node = node->parent;
            }
            this->m_rootNode = node;
            return node;
        }
    };

    template<typename T, typename U>
    struct  BRTree
    {
        BRTree(BRTree* _father, BRTree* _left, BRTree* _right, T _key, U _value, bool isleft, bool isred) : 
            father(_father),  left(_left), right(_right), key(_key), value(_value), isLeft(isleft), isRed(isred){}
        BRTree* father;
        BRTree* left;
        BRTree* right;

        T key;
        U value;

        bool isLeft;
        bool isRed;
    };
    
    template<typename T, typename U>
    class Map{
    public:    
        Map() = default;

        using Tree = BRTree<T, U>;

        void Insert(T key, U value) {
            auto insertNode = Insert(key, value, root);
            if (insertNode == nullptr) {
                return;
            }
            BalanceTree(insertNode);
            UpdateRoot(insertNode);
        }

        Tree* Insert(T key, U value, Tree* currentNode){
            if(root == nullptr){
                root = new Tree(nullptr, nullptr, nullptr, key, value, false, false);
                return nullptr;
            }
            Tree* NeedInsert = currentNode;

            if(key < NeedInsert->key){
                if (NeedInsert->left == nullptr) {
                    Tree* temp = new Tree(NeedInsert, nullptr, nullptr, key, value, true, true);
                    NeedInsert->left = temp;
                    return temp;
                }
                NeedInsert = NeedInsert->left;
                return Insert(key, value, NeedInsert);
            }
            if (key > NeedInsert->key) {
                if (NeedInsert->right == nullptr) {
                    Tree* temp = new Tree(NeedInsert, nullptr, nullptr, key, value, false, true);
                    NeedInsert->right = temp;
                    return temp;
                }
                NeedInsert = NeedInsert->right;
                return Insert(key, value, NeedInsert);
            }
            NeedInsert->value = value;
            return nullptr;
        }


    private:
        Tree* root{nullptr};

        void LeftRotate(Tree* node) {
            Tree* right = node->right;
            Tree* rightLeft = right->left;
            if (rightLeft != nullptr) {
                rightLeft->isLeft = false;

                rightLeft->father = node;
            }
            node->right = rightLeft;
            right->father = node->father;
            if (node->isLeft) {
                if (node->father != nullptr) {
                    node->father->left = right;
                }
                right->isLeft = true;
            }
            else {
                if (node->father != nullptr) {
                    node->father->right = right;
                }
                right->isLeft = false;
            }
            node->father = right;
            right->left = node;
            node->isLeft = true;
        }

        void RightRotate(Tree* node) {
            Tree* left = node->left;
            Tree* leftRight = left->right;
            if (leftRight != nullptr) {
                leftRight->isLeft = true;
                leftRight->father = node;
            }
            node->left = leftRight;
            left->father = node->father;
            if (node->isLeft) {
                if (node->father != nullptr) {
                    node->father->left = left;
                }
                left->isLeft = true;
            }
            else {
                if (node->father != nullptr) {
                    node->father->right = left;
                }
                left->isLeft = false;
            }
            left->right = node;
            node->father = left;
            node->isLeft = false;
            
        }

        void BalanceTree(Tree* node) {
            if (node->father->isRed == false) {
                return;
            }
            auto ppL = node->father->father->left;
            auto ppR = node->father->father->right;
            if (ppL != nullptr && ppR != nullptr &&
                ppL->isRed && ppR->isRed) {
                if (node->father->father != root) {
                    node->father->father->isRed = true;
                }
                node->father->isRed = false;
                return;
            }

            if (node->father->isLeft) {
                if (!node->isLeft) {
                    LeftRotate(node->father);
                    node->isRed = false;
                    node->father->isRed = true;
                    RightRotate(node->father);
                }
                else {
                    node->father->isRed = false;
                    node->father->father->isRed = true;
                    RightRotate(node->father->father);
                }
                return;
            }

            if (node->isLeft) {
                RightRotate(node->father);
                node->isRed = false;
                node->father->isRed = true;
                LeftRotate(node->father);
            }
            else {
                node->father->isRed = false;
                node->father->father->isRed = true;
                LeftRotate(node->father->father);
            }
        }

        void UpdateRoot(Tree* node) {
            if (node->father == nullptr) {
                root = node;
                return;
            }
            UpdateRoot(node->father);
        }
    };

} // namespace kTree
#endif