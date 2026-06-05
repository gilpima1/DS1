template<class Key, class Value>
struct Node {
    Key key;
    Value data;
    int height;
    Node<Key, Value>* left;
    Node<Key, Value>* right;

    Node(Key key, Value data, Node<Key, Value>* left = nullptr, Node<Key, Value>* right = nullptr) :
    key(key), data(data), height(1), left(left), right(right){}
    Node(Key key, Value data, int height, Node<Key, Value>* left = nullptr, Node<Key, Value>* right = nullptr) :
    key(key), data(data), height(height), left(left), right(right){}

    ~Node() = default;
};

template<class Key, class Value>
class AVL {
    Node<Key, Value>* root;

    void destroyTree(Node<Key, Value>* node = nullptr) {
        if (node == nullptr) {
            return;
        }

        if (node->left == nullptr && node->right == nullptr) {
            delete node;
            return;
        }

        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

    int getHeight(Node<Key, Value>* node) {
        if (node == nullptr) {return 0;}
        return node->height;
    }
    int getBalance(Node<Key, Value>* node) {
        if (node == nullptr) {return 0;}
        return getHeight(node->left) - getHeight(node->right);
    }

    Node<Key, Value>* rightRotate(Node<Key, Value>* y) {
        Node<Key, Value>* x = y->left;
        Node<Key, Value>* T2 = x->right;

        x->right = y;
        y->left = T2;

        int yLeftH = getHeight(y->left);
        int yRightH = getHeight(y->right);
        y->height = (yLeftH > yRightH ? yLeftH : yRightH) + 1;

        int xLeftH = getHeight(x->left);
        int xRightH = getHeight(x->right);
        x->height = (xLeftH > xRightH ? xLeftH : xRightH) + 1;

        return x;
    }
    Node<Key, Value>* leftRotate(Node<Key, Value>* x) {
        Node<Key, Value>* y = x->right;
        Node<Key, Value>* T2 = y->left;

        y->left = x;
        x->right = T2;

        int xLeftH = getHeight(x->left);
        int xRightH = getHeight(x->right);
        x->height = (xLeftH > xRightH ? xLeftH : xRightH) + 1;

        int yLeftH = getHeight(y->left);
        int yRightH = getHeight(y->right);
        y->height = (yLeftH > yRightH ? yLeftH : yRightH) + 1;

        return y;
    }
    Node<Key, Value>* balanceTree(Node<Key, Value>* node) {
        int balance = getBalance(node);
        if (balance == 2 && getBalance(node->left) >= 0) {
            return rightRotate(node);
        } else if (balance == 2 && getBalance(node->left) == -1) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        } else if (balance == -2 && getBalance(node->right) <= 0) {
            return leftRotate(node);
        } else if (balance == -2 && getBalance(node->right) == 1) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    Node<Key, Value>* insertNode(Node<Key, Value>* node, Key key, Value data) {
        if (node == nullptr) {return new Node<Key, Value>(key, data);}

        if (key < node->key){node->left = insertNode(node->left, key, data);}
        else if (key > node->key) {node->right = insertNode(node->right, key, data);}
        else {return node;}

        int leftH = getHeight(node->left);
        int rightH = getHeight(node->right);
        if (leftH > rightH) {
            node->height = leftH + 1;
        } else {
            node->height = rightH + 1;
        }

        return balanceTree(node);
    }
    Node<Key, Value>* findNode(Node<Key, Value>* node, Key key) {
        if (node == nullptr) {
            return nullptr;
        }

        if (node->key == key) {
            return node;
        }

        if (key < node->key) {
            return findNode(node->left, key);
        } else {
            return findNode(node->right, key);
        }
    }
    Node<Key, Value>* removeNode(Node<Key, Value>* node, Key key) {
        if (node == nullptr) {return node;}

        if (key < node->key) {
            node->left = removeNode(node->left, key);
        } else if (key > node->key) {
            node->right = removeNode(node->right, key);
        } else {
            if (node->left == nullptr) {
                Node<Key, Value>* temp = node->right;
                delete node;
                return temp;
            } else if (node->right == nullptr) {
                Node<Key, Value>* temp = node->left;
                delete node;
                return temp;
            } else {
                Node<Key, Value>* temp = node->right;
                while (temp->left != nullptr) {
                    temp = temp->left;
                }
                node->key = temp->key;
                node->data = temp->data;
                node->right = removeNode(node->right, temp->key);
            }
        }

        if (node == nullptr) {
            return node;
        }

        int leftH = getHeight(node->left);
        int rightH = getHeight(node->right);
        if (leftH > rightH) {
            node->height = leftH + 1;
        } else {
            node->height = rightH + 1;
        }

        return balanceTree(node);
    }

public:
    AVL() : root(nullptr){}

    ~AVL() {
        destroyTree(this->root);
    }

    void insert(Key key, Value data) {
        this->root = insertNode(this->root, key, data);
    }
    Node<Key, Value>* find(Key key) {
        if (this->root == nullptr) {
            return nullptr;
        }

        return findNode(this->root, key);
    }
    void remove(Key key) {
        this->root = removeNode(this->root, key);
    }
};