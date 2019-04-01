//
//  B_Plus_Tree.h
//  MiniSQL
//
//  Created by 梁潇 on 15/10/17.
//
//

//#pragma pack(1)

#ifndef B_Plus_Tree_h
#define B_Plus_Tree_h

#include <iostream>
#include <vector>
#include <math.h>
#include <stdio.h>
using namespace std;

#define DefaultDegree 100
#define POINT_TO_NOTHING 0xffffffffffffffff
typedef unsigned long FileAdr; //in 64bit-system, pointers is unsigned long(64bits).

#endif /* B_Plus_Tree_h */

struct index_offset
{
    unsigned int outer_offset;
    unsigned int inner_offset;
};

template <class KeyType>
class TreeNode
{
public:
    unsigned int degree; //define the max number of search a node can have
    unsigned int node_offset; //offset base on the root
    unsigned int count_searchkey;
    int isleaf; //true - leaf
    //size_t size_SearchKey;
    KeyType *node_searchkey;
    TreeNode<KeyType> **node_pointer;
    TreeNode<KeyType> *parent;
public:
    int calculate_node_size(int d);
public:
    //TreeNode();
    TreeNode(int Degree, int leaf_or_not);
    TreeNode(TreeNode &tn, int de);
    ~TreeNode();
};

template <class KeyType>
TreeNode<KeyType>::TreeNode(int Degree, int leaf_or_not) : degree(Degree), isleaf(leaf_or_not)
{
    node_searchkey = new KeyType[degree];
    node_pointer = new TreeNode<KeyType>*[degree + 1];
    for(int i=0; i<degree+1; i++)
        node_pointer[i] = NULL;
    count_searchkey = 0;
    parent = NULL;
    node_offset = 0;
};

template <class KeyType>
TreeNode<KeyType>::~TreeNode()
{
//    if(node_searchkey != NULL)
//        delete node_searchkey;
//    if(node_pointer != NULL)
//        delete node_pointer;
}

template <class KeyType>
TreeNode<KeyType>::TreeNode(TreeNode &tn, int de) : degree(de)
{
    int count_min;
    int i;
    node_searchkey = new KeyType[degree];
    node_pointer = new TreeNode<KeyType>*[degree + 1];
    if(tn.degree > de)
        count_min = de;
    else
        count_min = tn.degree;
    for(i=0; i<count_min; i++)
    {
        node_searchkey[i] = tn.node_searchkey[i];
        node_pointer[i] = tn.node_pointer[i];
    }
    node_pointer[i] = tn.node_pointer[i];
    node_pointer[de] = tn.node_pointer[tn.degree];
    parent = tn.parent;
    isleaf = tn.isleaf;
    count_searchkey = tn.count_searchkey;
}

template <class KeyType>
int TreeNode<KeyType>::calculate_node_size(int d)
{
    int s = sizeof(unsigned int) * 3 + sizeof(void *) * 3 + sizeof(int);
    s += sizeof(KeyType) * d;
    s += sizeof(TreeNode<KeyType>*) * (d + 1);
    return s;
}


template <class KeyType>
class B_Plus_Tree
{
    typedef TreeNode<KeyType>* Node;
public:
    string tname;
    string aname;
    string iname;
    unsigned int degree;
    unsigned int height;
    Node root;
    Node first_leaf_node;
    Node current_node;
    bool is_empty;
public:
    index_offset search_key(KeyType &key);
    bool insert_key(KeyType &key, FileAdr file_adr);
    bool delete_key(KeyType &key);
    bool drop_tree();


    bool read_from_disk();
    bool write_to_disk();
private:
    void insert_in_leaf(KeyType &key, Node aim, FileAdr file_adr);
    void insert_in_parent(KeyType &key, Node child1, Node child2);
    void delete_entry(KeyType &key, Node aim, unsigned int offset);
    int  find_key_index(KeyType &key, Node aim);
    void update_first_key(KeyType &keyp, KeyType &keyn, Node aim);
    void copy_node(Node dest, Node sour, int s1, int s2, int len);
    bool is_the_first_key(KeyType key);
    void write_node_to_disk(FILE **fp, int size, Node a);
public:
    void reset_block_offset(Node a);
    void print_tree(Node a);
    void rearrange_block(Node a, Node **buf, int s);
public:
    B_Plus_Tree(int Degree = DefaultDegree, string tn = " ", string inn=" ", string an=" ");
    //~B_Plus_Tree();
};

/*
 *Description: B+树构造函数
 */
template <class KeyType>
B_Plus_Tree<KeyType>::B_Plus_Tree(int Degree, string tn, string inn, string an) : degree(Degree), tname(tn), iname(inn), aname(an)
{
    root = new TreeNode<KeyType>(Degree, 0);
    for(int i=0; i<=Degree; i++)
    {
        root->node_pointer[i] = new TreeNode<KeyType>(Degree, 1);
        root->node_pointer[i]->node_offset = i + root->node_offset + 1;
        root->node_pointer[i]->parent = root;
    }
    current_node = root;
    first_leaf_node = root->node_pointer[1];
    is_empty = true;
    height = 2;
};

/*
 *Description: B+树查找函数,返回偏移地址结构，并且将current_node指向当年模块
 */
template <class KeyType>
index_offset B_Plus_Tree<KeyType>::search_key(KeyType &key)
{
    Node temp = NULL;
    index_offset result = {0, 0};
    current_node = root;
    reset_block_offset(root);
    int i = 0;

    if(root == NULL) //if the tree is empty
        return result;
    if(is_empty)
        return result;

    while(current_node->isleaf == 0) //not the leaf node
    {
        for(i=0; i<current_node->count_searchkey; i++) //find the next node(child)
        {
            if(key < current_node->node_searchkey[i])
            {
                break;
            }
        }
        temp = current_node;
        current_node = current_node->node_pointer[i];
        if(current_node == NULL)
            break;
    }
    if(current_node == NULL)
    {
        current_node = new TreeNode<KeyType>(this->degree, true);
        current_node->node_offset = current_node->parent->node_offset * degree + i + 1;
        current_node->parent = temp;
        result.outer_offset = current_node->node_offset;
        result.inner_offset = 0;
        return result;
    }
    for(i=0; i<current_node->count_searchkey; i++)
    {
        if(current_node->node_searchkey[i] == key && (FileAdr)current_node->node_pointer[i] != POINT_TO_NOTHING)
        {
            result.inner_offset = i;
            result.outer_offset = current_node->node_offset;
            break;
        }
    }
    return result;
};

/*
 *Description: B+树插入函数
 */
template <class KeyType>
bool B_Plus_Tree<KeyType>::insert_key(KeyType &key, FileAdr file_adr)
{
    index_offset temp;
    if(root == NULL)
    {
        return false;
    }
    else if(is_empty)
    {
        for(int i=0; i<=degree; i++)
        {
            root->node_pointer[i] = new TreeNode<KeyType>(degree, 1);
            root->node_pointer[i]->node_offset = i + root->node_offset + 1;
            root->node_pointer[i]->parent = root;
        }
        first_leaf_node = root->node_pointer[1];

        root->node_searchkey[0] = key;
        root->node_pointer[1] = first_leaf_node;
        first_leaf_node->node_searchkey[0] = key;
        is_empty = false;
        first_leaf_node->parent = root;
        root->count_searchkey++;
        first_leaf_node->count_searchkey++;
        return true;
    }
    else
    {
        temp = search_key(key); // current_node = the required leaf node
        if(temp.inner_offset != 0 || temp.outer_offset != 0) // if the key is already in the tree
        {
            if((FileAdr)current_node->node_pointer[temp.inner_offset] == POINT_TO_NOTHING)
            {
                current_node->node_pointer[temp.inner_offset] = (Node)file_adr;
            }
            return true;
        }
    }
    if(current_node->count_searchkey < current_node->degree)
    {
        insert_in_leaf(key, current_node, file_adr);
    }
    else if(current_node->count_searchkey == current_node->degree)
    {
        int i, splite_index;
        Node temp_node = new TreeNode<KeyType>(*current_node, current_node->degree + 1);
        Node new_node = new TreeNode<KeyType>(degree, 1);
        insert_in_leaf(key, temp_node, file_adr);
        splite_index = this->degree / 2;
        for(i=0; i<=splite_index; i++)
        {
            current_node->node_searchkey[i] = temp_node->node_searchkey[i];
            current_node->node_pointer[i] = temp_node->node_pointer[i];
        }
        //current_node->node_pointer[i] = temp_node->node_pointer[i];
        current_node->count_searchkey = splite_index + 1;
        current_node->node_pointer[this->degree] = new_node;
        for(i=splite_index+1; i<this->degree+1; i++)
        {
            new_node->node_searchkey[i - splite_index-1] = temp_node->node_searchkey[i];
            new_node->node_pointer[i - splite_index-1] = temp_node->node_pointer[i];
        }
        new_node->count_searchkey = this->degree - splite_index;
        new_node->node_pointer[this->degree] = temp_node->node_pointer[i];
        insert_in_parent(new_node->node_searchkey[0], current_node->parent, new_node);
        delete temp_node;
    }
    return true;
};

/*
 *Description: 键值插入叶节点中
 *Parameters: key:key值。aim:叶节点。 filf_adr: 文件地址
 */
template <class KeyType>
void B_Plus_Tree<KeyType>::insert_in_leaf(KeyType &key, Node aim, FileAdr file_adr)
{
    int i, j;
    for(i=0; i<aim->count_searchkey; i++)
    {
        if(key < aim->node_searchkey[i])
        {
            break;
        }
    }
    if(i == 0 && !is_the_first_key(aim->node_searchkey[0]))
    {
        update_first_key(aim->node_searchkey[0], key, aim->parent);
    }
    //aim->node_pointer[aim->count_searchkey+1] = aim->node_pointer[aim->count_searchkey];
    for(j=aim->count_searchkey; j>i; j--)
    {
        aim->node_searchkey[j] = aim->node_searchkey[j-1];
        aim->node_pointer[j] = aim->node_pointer[j-1];
    }
    aim->node_searchkey[i] = key;
    aim->node_pointer[i] = (Node)file_adr;
    aim->count_searchkey++;
}

template <class KeyType>
void B_Plus_Tree<KeyType>::insert_in_parent(KeyType &key, Node parent, Node child2)
{
    if(parent->count_searchkey < parent->degree)
    {
        int i, j;
        for(i=0; i<parent->count_searchkey; i++)
        {
            if(key < parent->node_searchkey[i])
            {
                break;
            }
        }
        //parent->node_pointer[arent->count_searchkey+1] = parent->node_pointer[arent->count_searchkey];
        for(j=parent->count_searchkey; j>i; j--)
        {
            parent->node_searchkey[j] = parent->node_searchkey[j-1];
            parent->node_pointer[j+1] = parent->node_pointer[j];
        }
        parent->node_searchkey[i] = key;
        parent->node_pointer[i+1] = child2;
        child2->parent = parent;
        parent->count_searchkey++;
    }
    else if(parent->count_searchkey == parent->degree)
    {
        int i, splite_index;
        Node temp_node = new TreeNode<KeyType>(*parent, parent->degree+1);
        Node new_node = new TreeNode<KeyType>(this->degree, false);
        insert_in_parent(key, temp_node, new_node);
        int m = find_key_index(key, temp_node);
        temp_node->node_pointer[m+1] = child2;
        splite_index = this->degree / 2;
        if(m<=splite_index)
            child2->parent = parent;
        else
            child2->parent = new_node;
        for(i=0; i<=splite_index; i++)
        {
            parent->node_searchkey[i] = temp_node->node_searchkey[i];
            parent->node_pointer[i] = temp_node->node_pointer[i];
        }
        parent->node_pointer[i] = temp_node->node_pointer[i];
        parent->count_searchkey = splite_index + 1;
        for(i=splite_index+1; i<this->degree+1; i++)
        {
            new_node->node_searchkey[i - splite_index-1] = temp_node->node_searchkey[i];
            new_node->node_pointer[i - splite_index] = temp_node->node_pointer[i+1];
        }
        //new_node->node_pointer[0] =
        new_node->count_searchkey = this->degree - splite_index;
        if(this->root != parent)
        {
            insert_in_parent(new_node->node_searchkey[0], parent->parent, new_node);
        }
        else
        {
            this->root = new TreeNode<KeyType>(this->degree, false);
            this->root->node_searchkey[0] = new_node->node_searchkey[0];
            this->root->node_pointer[0] = parent;
            this->root->node_pointer[1] = new_node;
            new_node->parent = this->root;
            this->root->count_searchkey = 1;
            this->height++;
        }
        new_node->node_pointer[0] = new_node->node_pointer[1];
        for(int i=0; i<new_node->count_searchkey - 1; i++)
        {
            new_node->node_searchkey[i] = new_node->node_searchkey[i+1];
            new_node->node_pointer[i+1] = new_node->node_pointer[i+2];
        }
        new_node->count_searchkey--;
        delete temp_node;
    }
}


template <class KeyType>
bool B_Plus_Tree<KeyType>::delete_key(KeyType &key)
{
    index_offset temp;
    if(root ==NULL || is_empty || root->count_searchkey == 0)
    {
        return false;
    }
    else
    {
        temp = search_key(key);
        if(temp.inner_offset == 0 && temp.outer_offset == 0) // no such key
        {
            return false;
        }
        current_node->node_pointer[temp.inner_offset] = (Node)POINT_TO_NOTHING;
    }
    return true;
}
// template <class KeyType>
// bool B_Plus_Tree<KeyType>::delete_key(KeyType &key)
// {
//     index_offset  temp;
//     if(root == NULL || is_empty || root->count_searchkey == 0)
//     {
//         return false;
//     }
//     else
//     {
//         temp = search_key(key); // after search, current_node points to the target
//         if(temp.inner_offset == 0 && temp.outer_offset == 0) // no such key
//         {
//             return false;
//         }
//     }
//     delete_entry(key, current_node, temp.inner_offset);
// }

/*
 *Description: 由于叶节点第一个key内容的变化，更新非叶节点的值
 *Parameters: keyp:旧的key值。keyn:要更新的key值。aim:要检查的第一个非叶节点
 */
template <class KeyType>
void B_Plus_Tree<KeyType>::update_first_key(KeyType &keyp, KeyType &keyn, Node aim)
{
    int j;
    if(aim == NULL)
    {
        return;
    }
    if((j = find_key_index(keyp, aim)) == -1)
    {
        update_first_key(keyp, keyn, aim->parent);
    }
    else
    {
        aim->node_searchkey[j] = keyn;
    }
}

/*
 *Description: 对某一块的某个key进行删除操作
 *Parameters: key：要删除的key值 aim：要删除键值所在的节点 offset：要删除键值在节点中的便宜位置
 */
template <class KeyType>
void B_Plus_Tree<KeyType>::delete_entry(KeyType &key, Node aim, unsigned int offset)
{
    //int i, j;
    if((aim->count_searchkey > (this->degree+1)/2) ||  (aim == this->root && aim->count_searchkey>1)) //no need for merge
    {
        //节点key数量够多（非根节点大于ceil(degree/2)或者根节点大于1），则直接删除
        if(aim->isleaf && offset == 0) // if the key is the first element, update the parent node
        {
            update_first_key(aim->node_searchkey[0],aim->node_searchkey[1], aim->parent);
        }
        // for(i=offset; i<aim->count_searchkey-1; i++)
        // {
        //     aim->node_searchkey[i] = aim->node_searchkey[i+1];
        //     aim->node_pointer[i] = aim->node_pointer[i+1];
        // }
        copy_node(aim, aim, offset, offset+1, aim->count_searchkey-1-offset);
        aim->count_searchkey--;
    }
    else if(aim->count_searchkey == 1 && (aim == root || aim->parent == root))
    {
        this->is_empty = true;
        delete root;
        root = new TreeNode<KeyType>(degree, 0);
        first_leaf_node = new TreeNode<KeyType>(degree, 1);
        current_node = root;
        root->node_pointer[1] = first_leaf_node;
        is_empty = true;
    }
    else
    {
        bool is_right_neighbor = true;
        Node next = aim->node_pointer[this->degree];
        if(next == NULL)
        {
            int k = find_key_index(aim->searchkey[0], aim->parent);
            next = aim->parent->node_pointer[k];
            is_right_neighbor = false;
        }
        if(next->count_searchkey > (this->degree+1)/2) //still two block
        {
            //当相邻节点的key数够多时，借用相邻节点的一个key
            if(!is_right_neighbor) // 向左结合
            {
                // for(int m =0; m<aim->count_searchkey; m++)
                // {
                //     aim->node_searchkey[m+1] = aim->node_searchkey[m];
                //     aim->node_pointer[m+1] = aim->node_pointer[m];
                // }
                copy_node(aim, aim, 1, 0, aim->count_searchkey);
                aim->node_searchkey[0] = next->node_searchkey[next->count_searchkey-1];
                aim->node_pointer[0] = next->node_pointer[next->count_searchkey-1];
                aim->count_searchkey++;
                update_first_key(aim->node_searchkey[0], aim->node_searchkey[1], aim->parent);
                delete_entry(key, aim, offset);
                delete_entry(next->node_searchkey[next->count_searchkey-1], next, next->count_searchkey-1);
            }
            else
            {
                aim->node_searchkey[aim->count_searchkey] = next->node_searchkey[0];
                aim->node_pointer[aim->count_searchkey] = next->node_pointer[0];
                aim->count_searchkey++;
                delete_entry(key, aim, offset);
                delete_entry(next->node_searchkey[0], next, 0);
            }
        }
        else // only one block left
        {
            //相邻节点数量不足，做块合并
            if(offset == 0) // if the key is the first element, update the parent node
            {
                update_first_key(aim->node_searchkey[0],aim->node_searchkey[1], aim->parent);
            }
            // for(int i=offset; i<aim->count_searchkey-1; i++)
            // {
            //     aim->node_searchkey[i] = aim->node_searchkey[i+1];
            //     aim->node_pointer[i] = aim->node_pointer[i+1];
            // }
            copy_node(aim, aim, offset, offset+1, aim->count_searchkey-offset-1);
            aim->count_searchkey--;
            if(!is_right_neighbor) // 向左结合
            {
                // for(int m=0; m<aim->count_searchkey; m++)
                // {
                //     next->node_searchkey[m+next->count_searchkey] = aim->node_searchkey[m];
                //     next->node_pointer[m+next->count_searchkey] = aim->node_pointer[m];
                // }
                copy_node(next, aim, next->count_searchkey, 0, aim->count_searchkey);
                next->count_searchkey += aim->count_searchkey;
                next->node_pointer[next->count_searchkey] = NULL;
                delete_entry(aim->node_searchkey[0], aim->parent, find_key_index(aim->node_searchkey[0], aim->parent));
                delete aim;
            }
            else
            {
                // for(int m=0; m<next->count_searchkey; m++)
                // {
                //     aim->node_searchkey[m+aim->count_searchkey] = next->node_searchkey[m];
                //     aim->node_pointer[m+aim->count_searchkey] = next->node_pointer[m];
                // }
                copy_node(aim, next, aim->count_searchkey, 0, next->count_searchkey);
                aim->count_searchkey += next->count_searchkey;
                aim->node_pointer[aim->count_searchkey] = next->node_pointer[next->count_searchkey];
                delete_entry(next->node_searchkey[0], next->parent, find_key_index(next->node_searchkey[0], next->parent));
                delete next;
            }
        }
    }
}

template <class KeyType>
int B_Plus_Tree<KeyType>::find_key_index(KeyType &key, Node aim)
{
    for(int i=0; i<aim->count_searchkey; i++)
    {
        if(key == aim->node_searchkey[i])
        {
            return i;
        }
    }
    return -1;
}

template <class KeyType>
void B_Plus_Tree<KeyType>::copy_node(Node dest, Node sour, int s1, int s2, int len)
{
    if(sour->isleaf)
    {
        for(int i=0; i<len; i++)
        {
            dest->node_searchkey[s1+i] = sour->node_searchkey[s2+i];
            dest->node_pointer[s1+i] = sour->node_pointer[s2+i];
        }
    }
    else
    {
        for(int i=0; i<len; i++)
        {
            dest->node_searchkey[s1+i] = sour->node_searchkey[s2+i];
            dest->node_pointer[s1+i+1] = sour->node_pointer[s2+i+1];
        }
        dest->node_pointer[s1] = sour->node_pointer[s2+1];
    }
}

/*
 *Description: 删除B+树
 */
template <class KeyType>
bool B_Plus_Tree<KeyType>::drop_tree()
{

}

template <class KeyType>
void B_Plus_Tree<KeyType>::print_tree(Node a)
{
    Node t = a;
    if(t->count_searchkey == 0 || (t != root && t->node_offset == 0))
        return;
    for(int i=0; i<t->count_searchkey; i++)
    {
        if(t->isleaf && (FileAdr)t->node_pointer[i] == POINT_TO_NOTHING)
            cout << t->node_searchkey[i] <<"$ ";
        else if(t->isleaf)
            cout << t->node_searchkey[i] <<"* ";
        else
            cout << t->node_searchkey[i] << ' ';
    }
    cout << "No." << t->node_offset << endl;
    if(t->isleaf)
        return;
    for(int i=0; i<=t->count_searchkey; i++)
    {
        if(t->node_pointer[i] == NULL || (FileAdr)t->node_pointer[i] == POINT_TO_NOTHING)
            continue;
        print_tree(t->node_pointer[i]);
    }
}

template <class KeyType>
bool B_Plus_Tree<KeyType>::is_the_first_key(KeyType key)
{
    Node t = root;
    while(!t->isleaf)
    {
        t = t->node_pointer[0];
    }
    if(key == t->node_searchkey[0])
        return true;
    return false;
}

template <class KeyType>
bool B_Plus_Tree<KeyType>::read_from_disk()
{
    FILE *fp;
    string path = "/Users/liangxiao/Documents/Xcodeworkspace/MiniSQL/MiniSQL/Data/" + tname + iname + ".idx";
    fp = fopen(path.c_str(), "rb+");
    if(fp == NULL)
    {
        return false;
    }
    fread(this, sizeof(B_Plus_Tree<KeyType>), 1, fp);
    fseek(fp, sizeof(B_Plus_Tree<KeyType>), SEEK_SET);
    int size = root->calculate_node_size(degree);

    int total = 0;
    for(int i=0; i<height; i++)
    {
        total += pow(degree, i);
    }
    Node *t = new Node[total];
    for(int i=0; i<total; i++)
    {
        unsigned char *tt;
        tt = new unsigned char[size];
        t[i] = (Node)tt;
    }
    int s = sizeof(unsigned int) * 3 + sizeof(void *) * 3 + sizeof(int);
    Node temp;
 //   temp = new unsigned char[s];
    unsigned char *tt;
    tt = new unsigned char[size];
    temp = (Node)tt;
    //memcpy(root, t[0], s);
    int ii = 0;

    while(!feof(fp) && ii<total)
    {
        size_t amount;
        amount = fread(temp, 1, size, fp);
        if(amount < size)
            break;
        memcpy(t[temp->node_offset], temp, size);
        //fread(t[temp->node_offset], size, 1, fp);
        //memcpy(t[temp->node_offset], temp, size);
        fseek(fp, size, SEEK_CUR);
        ii++;
    }


    root = (Node)t[0];
    root->isleaf = 0;
    //memcpy(root, t[0], s);
    delete temp;
    for(int i=total-1; i>=total - pow(degree, height-1); i--)
    {
        t[i]->isleaf = 1;
    }
    for(int i=0; i<total; i++)
    {
        Node tn = (Node)t[i];
//        tn->node_searchkey = new KeyType[degree];
//        memcpy(tn->node_searchkey, t[i]+s, sizeof(KeyType)*degree);
        tn->node_searchkey = (KeyType *)(((unsigned char *)t[i])+s);
        tn->node_pointer = (TreeNode<KeyType> **)(((unsigned char *)t[i]) + s + sizeof(KeyType) * degree);
        if(tn != root && tn->node_offset == 0)
            continue;
        if(tn != root && tn->count_searchkey == 0)
            continue;
        if(!tn->isleaf)
        {
            for(int i=0; i<degree + 1; i++)
            {
                tn->node_pointer[i] = t[tn->node_offset * degree + i + 1];
            }
        }
        if(tn == root)
            continue;
        tn->parent = (Node)t[(tn->node_offset-1)/degree];
    }
    //rearrange_block(root, t, s);
    //cout << "read index success!" <<endl;
    return true;
}

template <class KeyType>
bool B_Plus_Tree<KeyType>::write_to_disk()
{
    FILE *fp;
    string path = "/Users/liangxiao/Documents/Xcodeworkspace/MiniSQL/MiniSQL/Data/" + tname + iname + ".idx";
    fp = fopen(path.c_str(), "wb+");
    if(fp == NULL)
    {
        return false;
    }
    reset_block_offset(root);
    fwrite(this, sizeof(B_Plus_Tree<KeyType>), 1, fp);
    fseek(fp, sizeof(B_Plus_Tree<KeyType>), SEEK_SET);
    int size = root->calculate_node_size(degree);
    write_node_to_disk(&fp, size, root);
    fclose(fp);
    //cout << "Index write to disk success!" <<endl;
    return true;
}

template <class KeyType>
void B_Plus_Tree<KeyType>::write_node_to_disk(FILE **fp, int size, Node a)
{
    Node t = a;
    int s = sizeof(unsigned int) * 3 + sizeof(void *) * 3 + sizeof(int);
//    fwrite(t, s, 1, *fp);
//    fseek(*fp, s, SEEK_CUR);
//    s = sizeof(KeyType) * degree;
//    fwrite(t->node_searchkey , s, 1, *fp);
//    fseek(*fp, s, SEEK_CUR);
//    s = sizeof(TreeNode<KeyType>*) * (degree + 1);
//    fwrite(t->node_pointer, s, 1, *fp);
//    fseek(*fp, s, SEEK_CUR);
    unsigned char *tt = new unsigned char[size];
    unsigned char *ttt = tt;
    memcpy(ttt, t, s);
    ttt += s;
    s = sizeof(KeyType) * degree;
    memcpy(ttt, t->node_searchkey, s);
    ttt += s;
    s = sizeof(TreeNode<KeyType>*) * (degree + 1);
    memcpy(ttt, t->node_pointer, s);
    fwrite(tt, size, 1, *fp);
    fseek(*fp, size, SEEK_CUR);
    if(t->isleaf)
        return;
    for(int i=0; i<=t->count_searchkey; i++)
    {
        if(t->node_pointer[i] == NULL || (FileAdr)t->node_pointer[i] == POINT_TO_NOTHING || t->node_pointer[i]->count_searchkey == 0)
            continue;
        write_node_to_disk(fp, size, t->node_pointer[i]);
    }
}

template <class KeyType>
void B_Plus_Tree<KeyType>::reset_block_offset(Node a)
{
    Node t = a;
    if(t == root)
        t->node_offset = 0;
    if(t->isleaf)
        return;
    for(int i=0; i<=t->count_searchkey; i++)
    {
        if(t->node_pointer[i] == NULL || (FileAdr)t->node_pointer[i] == POINT_TO_NOTHING)
            continue;
        t->node_pointer[i]->node_offset = t->node_offset * degree + i + 1;
        reset_block_offset(t->node_pointer[i]);
    }
}

// template <class KeyType>
// void B_Plus_Tree<KeyType>::rearrange_block(Node a, Node **buf, int s)
// {
//     Node t = a;
//     //memcpy(t, buf[], s);
//     if(t->isleaf)
//         return;
//     for(int i=0; i<=t->count_searchkey; i++)
//     {
//         if(t->node_pointer[i] == NULL || (FileAdr)t->node_pointer[i] == POINT_TO_NOTHING)
//             continue;
//         rearrange_block(t->node_pointer[i], buf, s);
//     }
// }
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

//
//
//
//
//
//
//
