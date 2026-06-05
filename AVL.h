struct Node {
    int data;
    int height;
    Node* left;
    Node* right;

    Node(int data, Node* left = nullptr, Node* right = nullptr) : data(data), height(1), left(left), right(right){}
    Node(int data, int height, Node* left = nullptr, Node* right = nullptr) : data(data), height(height), left(left), right(right){}

    ~Node() = default;
};

class AVL {
    Node* root;

    void destroyTree(Node* node = nullptr) {
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

    int getHeight(Node* node) {
        if (node == nullptr) {return 0;}
        return node->height;
    }
    int getBalance(Node* node) {
        if (node == nullptr) {return 0;}
        return getHeight(node->left) - getHeight(node->right);
    }

    Node* rightRotate(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;

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
    Node* leftRotate(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;

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
    Node* balanceTree(Node* node) {
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

    Node* insertNode(Node* node, int data) {
        if (node == nullptr) {return new Node(data);}

        if (data <= node->data){node->left = insertNode(node->left, data);}
        else {node->right = insertNode(node->right, data);}

        int leftH = getHeight(node->left);
        int rightH = getHeight(node->right);
        if (leftH > rightH) {
            node->height = leftH + 1;
        } else {
            node->height = rightH + 1;
        }

        return balanceTree(node);
    }
    bool findNode(Node* node, int data) {
        if (node == nullptr) {
            return false;
        }

        if (node->data == data) {
            return true;
        }

        if (data <= node->data) {
            return findNode(node->left, data);
        } else {
            return findNode(node->right, data);
        }
    }
    Node* removeNode(Node* node, int data) {
        if (node == nullptr) {return node;}

        if (data < node->data) {
            node->left = removeNode(node->left, data);
        } else if (data > node->data) {
            node->right = removeNode(node->right, data);
        } else {
            if (node->left == nullptr) {
                Node* temp = node->right;
                delete node;
                return temp;
            } else if (node->right == nullptr) {
                Node* temp = node->left;
                delete node;
                return temp;
            } else {
                Node* temp = node->right;
                while (temp->left != nullptr) {
                    temp = temp->left;
                }
                node->data = temp->data;
                node->right = removeNode(node->right, temp->data);
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
    AVL(int data) : root(new Node(data)){}

    ~AVL() {
        destroyTree(this->root);
    }

    void insert(int data) {
        this->root = insertNode(this->root, data);
    }
    bool find(int data) {
        if (this->root == nullptr) {
            return false;
        }

        return findNode(this->root, data);
    }
    void remove(int data) {
        this->root = removeNode(this->root, data);
    }
};