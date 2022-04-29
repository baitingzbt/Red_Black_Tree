#ifndef rbt_h
#define rbt_h
#include <utility>
#include <functional>
#include <stdexcept>
#include <iostream>

/**
 @tparam T is the data stored in the rbt
 @tparam compare_type is the rule to compare node values (of type T)
 node is the nested class
 root is a pointer to a node, initialzed to nullptr
 pred is the correspoding compare_type
 tree_size records the number of elements in the tree
*/
template< typename T, typename compare_type = std::less< T > >
class rbt;

/**
 non-member swap functionthat stays outside of pic10c namespace
 @param tree1 is the "left hand side" rbt
 @param tree2 is the "right hand side" rbt
*/
template< typename T, typename compare_type >
void swap(rbt<T, compare_type>& tree1, rbt<T, compare_type>& tree2) { tree1.swap(tree2); }


template< typename T, typename compare_type >
class rbt
{
private:
    /**
     the definition of ndoe class, which is nexted within rbt
     value is the valued  stored in the node of type T
     pred is the comparing mechanism for the nodes' values
     left, right, parent all all points to its left-child node, right-child node, and parent-node correspondingly
     */
    class node;
    node* root = nullptr;
    compare_type pred;
    size_t tree_size = 0;
    
    /**
     Insert a node traversely, until it finds it position
     @param start is a pointer to a node
    */
    void traverse_insert(node* start);

    /**
     Traversely delete nodes from a position, and every nodes below that position. Currently, this is not color-fitted, so only called when deleting the entire tree (destructor)
     @param start is the node to below which everything are removed
    */
    void traverse_delete(node* start);

    
public:
    /**
     default constructor of rbt
     @param _pred is the given compare type
    */
    rbt(const compare_type& _pred = compare_type()) noexcept : pred(_pred) { }
    
    /**
     copy constructor of rbt
     @param other is another rbt named
    */
    rbt(const rbt& other) : pred(other.pred) { traverse_insert(other.root); } // copy constructor
    
    /**
     move constructor of rbt
     @param obj is another rbt
     */
    rbt(rbt&& obj) noexcept : rbt()
    {
        using std::swap;
        swap(root, obj.root);
        swap(pred, obj.pred);
        swap(tree_size, obj.tree_size);
        obj.root = nullptr; // remove the other rbt after swapping
    }      //move constructor
    
    /**
     destructor, traversely delete everythoing from the root
     */
    ~rbt() { if (root) { traverse_delete(root); } }
    
    /**
     equal operator of rbt
     @param other is rbt type on thr right hand side
     @return a rbt reference
    */
    rbt& operator=(rbt other) &
    {
        rbt temp = copy(other); // calls the copy constructor
        using std::swap;
        swap(root, temp.root);
        swap(pred, temp.pred);
        swap(tree_size, temp.tree_size);
    }
    
    /**
     the definition of iterator class, which is nexted within rbt
     this node is the node that iterator pointing to
     container is the rbt tree that iterator belongs to
     pred is the comparing type for the templated T values passed in
     */
    class iterator; // nested iterator class
    
    class const_iterator;
    
    /**
     find the begin iterator of a tree, which is the smallest position
    @return an iterator to the begin() or smallest of the rbt
    */
    iterator begin(); // begin() "smallest" of the rbt
    const_iterator begin() const;
    
    /**
     the end means "past the end" iterator, which always points to null
    @return an iterator to the end() or largest of the rbt
    */
    iterator end(); // end() "largest" of the rbt
    const_iterator end() const;
    
    iterator largest();
    const_iterator largest() const;
    
    /**
     insert function for rbt, attempts to insert a value of templated T type (l-value)
    @param other is a l value of type T
    */
    void insert(const T& other); // insert by l value
    
    /**
     insert function for rbt, attempts to insert a value of templated T type (r-value overload)
    @param other is a r value of type T, overried
    */
    void insert(T&& other); // insert by r value

    /**
     locate a node in the rbt structure
     @param other is a node to identify in rbt
     @return if found, return its iterator; if not found, return the null iterator
    */
    iterator find(const node& other)
    {
        // simply go through each element to return the corresponding iterator, if not found ,returne null iterator
        iterator current = begin();
        // as long as not reaching the end, do so recurssively
        while (current != end())
        {
            // directly return the iterator if found matching value
            if (current.this_node->get_node_val() == other.value) { return current; }
            // keep doing while, nothing special if vvalue not matching
            else { ++current; }
        }
        return iterator(nullptr, this);
    }
    
    /**
     locate a value in the rbt structure
     @param value is a node to identify in rbt
     @return if found, return its iterator; if not found, return the null iterator
    */
    iterator find(const T& value)
    {
        // simply go through each element to return the corresponding iterator, if not found ,returne null iterator
        iterator current = begin();
        // as long as not reaching the end, do so recurssively
        while (current != end())
        {
            // directly return the iterator if found matching value
            if (current.this_node->get_node_val() == value) { return current; }
            // keep doing while, nothing special if vvalue not matching
            else { ++current; }
        }
        return iterator(nullptr, this);
    }
    
    /**
     find the size of the rbt
     @return a positive integer or 0 indicating the number of nodes of the rbt
     */
    const size_t size() { return tree_size; }
    
    /**
     member emplace function that inputs the given arguments into the templated type and put into rbt
     @tparam Args are the arguments passed in to emplace together
    */
    template <typename... Args>
    void emplace(Args&&... values); // member emplace function
    
    /**
     erase function for rbt, given an iterator
    @param iter is an iterator to identify in the rbt
     */
    void erase(iterator iter); //erase a specific iterator
    
    /**
    A member version of swap function to swap current rbt with the given one
    @param other is rbt  to swap with
    */
    void swap(rbt& other); // member swap function
    
    /**
     Find how many steps from the start it takes to reach the current node, now only using start as root.
     @param start is the starting point to calculate height
     @param target is the ending point
     @param cumulated_height is a number passed into the function so that it calls recurssively, always first start from 0!
     @return an integer, indicating length from start to target
     */
    const int node_depth(node* start, node* target, int cumulated_height);
    
    /**
     The function used to print the structure of the tree
     */
    void print();
};

template< typename T, typename compare_type >
class rbt<T, compare_type>::node
{
    friend rbt; // made friend so rbt and iterator types can access the node values, etc
    friend iterator;
    friend const_iterator;
private:
    T value;
    compare_type pred;
    node* left; // left child
    node* right; // right child
    node* parent; // parent node
    std::string color; // color of the node
    
    /**
    default constructor of the node class, no parameter, default point things to nullptr
    */
    node();
    
    /**
     construct node given its value, but has no specified left, right, nor parent
    @param val is a variable of type T
    */
    node(T val, std::string col);
public:
    /**
     the public method to get node value
     @return the value stored in the node
     */
    const T get_node_val() const { return value; }
    
    /**
     insert node function for node, trying to insert at the node's right and left, if not empty, then do so recursively
    @param new_node is a pointer to a node is
    @param _pred is the cooreponding compare type for values
    */
    void insert_node(node* new_node, compare_type _pred); // insert node at node member function
    
    /**
     find what kind of child the current node has
     @return a const string, which is "left_only", "right_only", "none", or "both"
     */
    const std::string find_its_child();
    
    /**
     rotate left about the current node, only changes connection
     */
    void left_rotate();
    
    /**
     rotate right about the current node, only changes connection
     */
    void right_rotate();
    
    /**
     find whether this node is the left child of the parent, right child of parent, or the root
     @return a const string, must be one of "left", "right", or "root"
     */
    const std::string find_node_position();
    
    /**
     find the sibling of the node
     @return a pointer to this node's sibling, can be a nullptr if the node doesn't have a sibling
     */
    node* find_node_sibling();
    
    /**
     helper function to correct coloring during erase, starting from this node
     */
    void correct_color_erase();
    
    /**
     helper function to correct coloring during insert, starting from this node
     @param new_node_pos is whether during inserting, the new node is trying to be inserted to this node's left or right
     */
    void correct_color_insert(std::string new_node_pos);
}; // end of node class


template< typename T, typename compare_type >
void rbt<T, compare_type>::traverse_insert(node* start)
{
    // only insert when not starting from null
    if (start!=nullptr)
    {
        // insert recursively, everything to the left and right of the node, and the node itself
        insert(start->value);
        // if there are something on the left, go start recurssively from there
        if (start->left != nullptr) { traverse_insert(start->left); }
        // if there are something on the right, go start recurssively from there
        if (start->right != nullptr) { traverse_insert(start->right); }
    }
    // if starting from null, throw error
    else { throw; }
}

template< typename T, typename compare_type >
void rbt<T, compare_type>::traverse_delete(node* start)
{
    // only works when not starting from null
    if (start != nullptr) {
        // if something is on the left, first go delete there
        if ( (start->left) != nullptr ) { traverse_delete(start->left); }
        // if something is on the right, first go delete there
        if ( (start->right) != nullptr ) { traverse_delete(start->right); }
        --tree_size;
        start = nullptr;
        delete start;
    }
    // if starting from null, throw error
    else { throw; }
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::iterator rbt<T, compare_type>::largest()
{
    if (root == nullptr) { return iterator(nullptr, this); } // if there is no rrot return nullptr, else go to next line
    iterator iter = iterator(root, this);
    while (iter.this_node->right != nullptr) { iter.this_node = iter.this_node->right; } // always go the farthest right and return the iterator
    return iter;
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::const_iterator rbt<T, compare_type>::largest() const
{
    if (root == nullptr) { return const_iterator(nullptr, this); } // if there is no rrot return nullptr, else go to
    const_iterator iter = const_iterator(root, this);
    while (iter.this_node->right != nullptr) { iter.this_node = iter.this_node->right; } // always go the farthest right and return the iterator
    return iter;
}

template< typename T, typename compare_type >
template < typename... Args >
void rbt<T, compare_type>::emplace(Args&&... values)
{
    // create a new node with the correct type, first initialze to unknown color
    node* new_node = new node(T(std::forward< Args > (values) ...), "unknown");

    ++tree_size;
    // insert the new node into the tree
    // insert to root if there isn't one yet
    if (root == nullptr) { new_node->color = "black"; root = new_node; }
    // if there is a root, insert_node called recurssively from there
    else { new_node->color = "red"; root->insert_node(new_node, pred); }
}

template< typename T, typename compare_type >
const int rbt<T, compare_type>::node_depth(node* current, node* target, int cumulated_height)
{
    const bool is_this = current->value == target->value; // if current node equals target node
    const bool is_on_left = pred(target->value, current->value); // if target is on the left of current
    const bool is_on_right = pred(current->value, target->value); // if target is on the right of current
    
    if (is_this) { return cumulated_height; } // return cumulated height until now
    else if (is_on_left) { return node_depth(current->left, target, ++cumulated_height); } // add 1 to height, and to left
    else if (is_on_right) { return node_depth(current->right, target, ++cumulated_height); } // add 1 to height, and to right
    else { return -1; } // if none of these cases, return -1 as a special notation
}


template< typename T, typename compare_type >
const std::string rbt<T, compare_type>::node::find_its_child()
{
    std::string its_child;
    if (left == nullptr && right == nullptr) { its_child = "none"; }
    else if (left == nullptr && right != nullptr) { its_child = "right_only"; }
    else if (left != nullptr && right == nullptr) { its_child = "left_only"; }
    else { its_child = "both"; }
    return its_child;
}

template< typename T, typename compare_type >
void rbt<T, compare_type>::node::left_rotate()
{
    right->parent = parent;
    if (parent != nullptr) // if there is a parent, not root
    {
        if (parent->left != nullptr) // if parent's left is not nullptr
        {
            if (parent->left == this) { parent->left = right; } // if this is the left child of parent
            else { parent->right = right; } // if this is the right child of parent
        }
        else { parent->right = right; } // if parent's left is nullptr, then this must be the parent's right child
    } // if target is root, do the following directly, no need figure out new parent
    parent = right; // new parent is its old right child
    if (right->left != nullptr) // find new right child if exist
    {
        right->left->parent = this;
        right = right->left;
    }
    else { right = nullptr; } // connect right child to null otherwise
    parent->left = this;
}

template< typename T, typename compare_type >
void rbt<T, compare_type>::node::right_rotate()
{
    left->parent = parent;
    if (parent != nullptr) // if there is a parent, not root
    {
        if (parent->left != nullptr) // if parent's left is not nullptr
        {
            if (parent->left == this) { parent->left = left; } // if this is the left child of parent
            else { parent->right = left; } // if this is the right child of parent
        }
        else { parent->right = left; } // if parent's left is nullptr, then this must be the parent's right child
    } // if target is root, do the following directly, no need figure out new parent
    parent = left;// new parent is its old right child
    if (left->right != nullptr) // find new right child if exist
    {
        left->right->parent = this;
        left = left->right;
    }
    else { left = nullptr; } // connect right child to null otherwise
    parent->right = this;
}

template< typename T, typename compare_type >
const std::string rbt<T, compare_type>::node::find_node_position()
{
    if (parent == nullptr) {  return "root"; } // parent is null means this node is root
    else if (parent->right == nullptr) {  return "left"; } // if parent's right is null, this node is left
    else if (parent->left == nullptr) { return "right"; } // if parent's left is null, this node is right
    else if (this == parent->left) { return "left"; } // if parent's left and right both not null, check equality
    else { return "right"; } // not equal to left means must be right
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::node* rbt<T, compare_type>::node::find_node_sibling()
{
    std::string node_position = find_node_position();
    if (node_position == "root") { return nullptr; } // root must have no sibling
    else if (node_position == "left") { return parent->right; } // left node has right sibling, may be null sibling
    else { return parent->left; } // right node has left sibling, may be null sibling
}

template< typename T, typename compare_type >
void rbt<T, compare_type>::node::correct_color_erase()
{
    const std::string its_child = find_its_child(); // its_child is "none", "left_only", "right_only"| even if "both" happens, it is not meaningful!
    
    std::string node_position = find_node_position();
    node* sibling = find_node_sibling();
    std::string sibling_color = "black";
    if (sibling != nullptr && sibling->color == "red") { sibling_color = "red"; } // sibling color is defaulted black, unless it has a non null red sibling
    
    node* left_child_of_sibling = sibling->left;
    node* right_child_of_sibling = sibling->right;
    std::string col_of_left_of_sibling = "black";
    if (left_child_of_sibling != nullptr && left_child_of_sibling->color == "red") { col_of_left_of_sibling = "red"; } // color of left of sibling is defaulted black, unless it has a non null and red left of sibling
    std::string col_of_right_of_sibling = "black";
    if (right_child_of_sibling != nullptr && right_child_of_sibling->color == "red") { col_of_right_of_sibling = "red"; } // color of right of sibling is defaulted black, unless it has a non null and red right of sibling
    
    if (color == "double_black") // if there is a double black, run this part to "fix" it first!
    {
        if (node_position == "root") { color = "black"; } // if this is the node, guarantee to color it black
        else if (sibling_color == "red" && node_position == "left") // red sibling means parent must be black
        {
            parent->color = "red";
            parent->left_rotate();
            correct_color_erase();
        }
        else if (sibling_color == "red" && node_position == "right") // red sibling means parent must be black
        {
            parent->color = "red";
            parent->right_rotate();
            correct_color_erase();
        }
        else if (sibling_color == "black" && node_position == "left") // black sibling, n is left child
        {
            if (col_of_left_of_sibling == "red" && col_of_right_of_sibling == "black") // sibling has left-child red, right-child black
            {
                using namespace std;
                std::swap(sibling->color, left_child_of_sibling->color);
                sibling->right_rotate();
                correct_color_erase();
            }
            else if (col_of_right_of_sibling == "red") // sibling has right-child red, left-child any color
            {
                color = "black";
                right_child_of_sibling->color = "black";
                if (parent->color == "red") // parent is red, correct color and rotate
                {
                    sibling->color = "red";
                    parent->color = "black";
                }
                else { } // parent is black, just need to rotate
                parent->left_rotate();
            }
            else // color of both children of sibling are black
            {
                color = "black";
                if (parent->color == "red") // parent is red, correct colors and done
                {
                    parent->color = "black";
                    sibling->color = "red";
                }
                else // parent is black, correct color and call recurssively
                {
                    parent->color = "double_black";
                    sibling->color = "red";
                    parent->correct_color_erase();
                }
            }
        }
        else if (sibling_color == "black" && node_position == "right") // black sibling, n is right child
        {
            
            if (col_of_left_of_sibling == "black" && col_of_right_of_sibling == "red") // sibling has one left black and one right red child
            {
                using namespace std;
                std::swap(sibling->color, right_child_of_sibling->color);
                sibling->left_rotate();
                correct_color_erase();
            }
            else if (col_of_left_of_sibling == "red") // sibling has left red child (black is whatever child)
            {
                color = "black";
                left_child_of_sibling->color = "black";
                if (parent->color == "red") // IV, A
                {
                    sibling->color = "red";
                    parent->color = "black";
                }
                else { } // IV, B
                parent->right_rotate();
            }
            else // color of both children of sibling are black
            {
                color = "black";
                if (parent->color == "red") // red parent, correct color
                {
                    parent->color = "black";
                    sibling->color = "red";
                }
                else // black parent, correct based upon its parent
                {
                    parent->color = "double_black";
                    sibling->color = "red";
                    parent->correct_color_erase();
                }
            }
        }
    }
    
    if (its_child == "none") // if no child, just correct color and call recurssively
    {
        if (color == "red") { color = "black"; } // removal 2, red and no children
        else // removal 3, black and no children
        {
            color = "double_black";
            correct_color_erase();
        }
    }
    else if (its_child == "right_only" && right->color == "red") { right->color = "black"; } // only right red child means color its right child black
    else if (its_child == "left_only" && left->color == "red") { left->color = "black"; } // only left red child means color its left child black
    else { } // if having both child, the erase method wouldn't do anything first but to swap values, and then call erase recurssively, so no need to correct their colors since not changing any connection
}

template< typename T, typename compare_type >
void rbt<T, compare_type>::node::correct_color_insert(std::string new_node_pos)
{
    node* new_node;
    if (new_node_pos == "left") { new_node = left; }
    else { new_node = right; } // find how to connect the new node to the original ones
    
    if (parent == nullptr) { color = "black"; } // if parent is null, this is root, color this to black
    if (color == "black") { } // if inserting under black, need to do thing
    else if (color == "red") // if inserting under red
    {
        std::string sibling_color = "black";
        if (find_node_sibling() != nullptr && find_node_sibling()->color == "red") { sibling_color = "red"; }
        // first find sibling color, which default to be black, underless sibling is not null and red
        if (sibling_color == "red") // if has a red sibling, just correct the colors
        {
            parent->left->color = "black";
            parent->right->color = "black";
            parent->color = "red";
            //parent->correct_color_insert(parent->find_node_position()); // correction upon grandparent
        }
        else // sibling is black
        {
            std::string pos = find_node_position();
            if (new_node_pos == "left" && pos == "left") // if new node is append to left and this node is on the left of parent
            {
                color = "black";
                parent->color = "red";
                parent->right_rotate();
            }
            else if (new_node_pos == "right" && pos == "right") // if new node is append to right and this node is on the right of parent
            {
                color = "black";
                parent->color = "red";
                parent->left_rotate();
            }
            else if (new_node_pos == "right" && pos == "left") // if new node is append to right and this node is on the left of parent
            {
                left_rotate();
                correct_color_insert(pos); // correction upon parent
            }
            else // if new node is append to left and this node is on the right of parent
            {
                right_rotate();
                correct_color_insert(pos); // correction upon parent
            }
        }
    }
}

template< typename T, typename compare_type >
class rbt<T, compare_type>::iterator
{
    friend rbt; // made friend so rbt can access the iterator's functions and everything
private:
    node* this_node; // the node that iterator points to
    const rbt* container; // the rbt the iterator belongs to
    compare_type pred;
    iterator() : this_node(nullptr), container(nullptr) { } // default point to nullptrs
    iterator(node* other, const rbt* rbt) : this_node(other), container(rbt) { } // constructor given node and a tree
public:
    /**
     find the next node of the current iterator, returns a pointer to node
     @return return a pointer to a node that is the next
    */
    node* find_next_node(); // find the next bigger node after this iterator

    /**
     find the previous node of the current iterator, returns a pointer to node
     @return return a pointer to a node that is the previous
    */
    node* find_previous_node(); // find the next smaller node after this iterator
    
    /**
     prefix increment for iterator
    */
    iterator& operator++();
    
    /**
     postfix increment for iterator
    */
    iterator operator++(int);
    
    /**
     prefix decrement for iterator
    */
    iterator& operator--();
    
    /**
     postfix decrement for iterator
    */
    iterator operator--(int);
    
    /**
     overload dereference operator for iterators, simply return the value of its node of T reference
    @return the value reference of where the iterator is pointing to
    */
    const T& operator*() const;
    
    /**
     overload arrow operator for iterators, returns the deference value of T type
    @return the reference or pointer to the value of where the iterator is pointing to
    */
    const T* operator->() const;
    
    /**
     overload the output operator for iterator class
     @param iter the iterator pointing the value want to print out
     */
    friend std::ostream& operator<<(std::ostream &out, iterator iter) { return out << iter.this_node->value; }
    
    /**
     overload double equal operator to find whether two iterators are the same
    @param other is the right hand side of the double equal operator
    @return a bool type of whether this current rbt and the given are equal
    */
    bool operator==(iterator other) const;
    
    /**
     overload not equal operator to find whether two iterators are not equal
    @param other is the right hand side of the not equal operator
    @return a bool type of whether this current rbt and the given are not equal
    */
    bool operator!=(iterator other) const;
    
    /**
     the member function for iterator to print the node it points to. This prints the value, the color, and whether it is a left or right child (or root) of its parent, along with the structural padding (spaces)
     @param depth_padding is how much extra padding
     */
    void print_iter_node(const std::string& depth_padding);
}; // end of iterator class

template< typename T, typename compare_type >
class rbt<T, compare_type>::const_iterator
{
    friend rbt; // made friend so rbt can access the iterator's functions and everything
private:
    node* this_node; // the node that iterator points to
    const rbt* container; // the rbt the iterator belongs to
    compare_type pred;
    const_iterator() : this_node(nullptr), container(nullptr) { } // default point to nullptrs
    const_iterator(node* other, const rbt* rbt) : this_node(other), container(rbt) { } // constructor given node and a tree
public:
    /**
     find the next node of the current iterator, returns a pointer to node
     @return return a pointer to a node that is the next
    */
    node* find_next_node(); // find the next bigger node after this iterator
    
    /**
     find the previous node of the current iterator, returns a pointer to node
     @return return a pointer to a node that is the previous
    */
    node* find_previous_node(); // find the next smaller node after this iterator
    
    /**
     prefix increment for iterator
    */
    const_iterator& operator++();
    
    /**
     postfix increment for iterator
    */
    const_iterator operator++(int);
    
    /**
     prefix decrement for iterator
    */
    const_iterator& operator--();
    
    /**
     postfix decrement for iterator
    */
    const_iterator operator--(int);
    
    /**
     overload dereference operator for iterators, simply return the value of its node of T reference
    @return the value reference of where the iterator is pointing to
    */
    const T& operator*() const;
    
    /**
     overload arrow operator for iterators, returns the deference value of T type
    @return the reference or pointer to the value of where the iterator is pointing to
    */
    const T* operator->() const;
    
    /**
     overload the output operator for iterator class
     @param iter the iterator pointing the value want to print out
     */
    friend std::ostream& operator<<(std::ostream& out, const_iterator iter) { return out << iter.this_node->value; }
    
    /**
     overload double equal operator to find whether two iterators are the same
    @param other is the right hand side of the double equal operator
    @return a bool type of whether this current rbt and the given are equal
    */
    bool operator==(const_iterator other) const;
    
    /**
     overload not equal operator to find whether two iterators are not equal
    @param other is the right hand side of the not equal operator
    @return a bool type of whether this current rbt and the given are not equal
    */
    bool operator!=(const_iterator other) const;
    
    /**
     the member function for iterator to print the node it points to. This prints the value, the color, and whether it is a left or right child (or root) of its parent, along with the structural padding (spaces)
     @param depth_padding is how much extra padding
     */
    void print_iter_node(const std::string& depth_padding) const;
}; // end of const iterator class


template< typename T, typename compare_type >
void rbt<T, compare_type>::swap(rbt& other)
{
    // swapping the root, pred, and size are effectively swapping the trees
    using std::swap;
    swap(root, other.root);
    swap(pred, other.pred);
    swap(tree_size, other.tree_size);
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::node* rbt<T, compare_type>::iterator::find_next_node() // find the node, whose value is the next larger one than the given node
{
    node* current_node = this_node;
    if (current_node->right != nullptr)// if something is on the right, go to the next leftest node
    {
        current_node = current_node->right;
        // keep going to the farthest left of the right
        while (current_node->left != nullptr) { current_node = current_node->left; }
        return current_node;
    }
    else // if nothing is on the right, then already at the largest value
    {
        std::string node_position = current_node->find_node_position();
        // keep going to find whether this is coming from a left child
        while (current_node->parent != nullptr)
        {
            if (node_position == "left") { return current_node->parent; } // came from left the return parent
            else // otherwise keep going
            {
                current_node = current_node->parent;
                node_position = current_node->find_node_position();
            }
        }
        return nullptr; //return null is it is the largest
    }
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::node* rbt<T, compare_type>::const_iterator::find_next_node() // find the node, whose value is the next larger one than the given node
{
    node* current_node = this_node;
    if (current_node->right != nullptr)// if something is on the right, go to the next leftest node
    {
        current_node = current_node->right;
        // keep going to the farthest left of the right
        while (current_node->left != nullptr) { current_node = current_node->left; }
        return current_node;
    }
    else // if nothing is on the right, then already at the largest value
    {
        std::string node_position = current_node->find_node_position();
        // keep going to find whether this is coming from a left child
        while (current_node->parent != nullptr)
        {
            if (node_position == "left") { return current_node->parent; } // came from left the return parent
            else // otherwise keep going
            {
                current_node = current_node->parent;
                node_position = current_node->find_node_position();
            }
        }
        return nullptr; //return null is it is the largest
    }
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::node* rbt<T, compare_type>::iterator::find_previous_node()
{
    node* current_node = this_node;
    if (current_node->left != nullptr)// if something is on the left, go to the next rightest node
    {
        current_node = current_node->left;
        // keep going to the fartherest right of its left
        while (current_node->right != nullptr) { current_node = current_node->right; }
        return current_node;
    }
    else // if nothing is on the right, then already at the largest value
    {
        std::string node_position = current_node->find_node_position();
        // keep going to find whether this is coming from a right child
        while (current_node->parent != nullptr)
        {
            if (node_position == "right") { return current_node->parent; } // came from right the return parent
            else // otherwise keep going
            {
                current_node = current_node->parent;
                node_position = current_node->find_node_position();
            }
        }
        return nullptr; //return null is it is the smallest
    }
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::node* rbt<T, compare_type>::const_iterator::find_previous_node()
{
    const node* current_node = this_node;
    if (current_node->left != nullptr)// if something is on the left, go to the next rightest node
    {
        current_node = current_node->left;
        // keep going to the fartherest right of its left
        while (current_node->right != nullptr) { current_node = current_node->right; }
        return current_node;
    }
    else // if nothing is on the right, then already at the largest value
    {
        std::string node_position = current_node->find_node_position();
        // keep going to find whether this is coming from a right child
        while (current_node->parent != nullptr)
        {
            if (node_position == "right") { return current_node->parent; } // came from right the return parent
            else // otherwise keep going
            {
                current_node = current_node->parent;
                node_position = current_node->find_node_position();
            }
        }
        return nullptr; //return null is it is the smallest
    }
}

template< typename T, typename compare_type >
rbt<T, compare_type>::node::node() : left(nullptr), right(nullptr), parent(nullptr) { }

template< typename T, typename compare_type >
rbt<T, compare_type>::node::node(T val, std::string col) : left(nullptr), right(nullptr), parent(nullptr), value(val), color(col) { }

template< typename T, typename compare_type >
void rbt<T, compare_type>::node::insert_node(node* new_node, compare_type _pred)
{
    // insert node to the farthest left if it is smaller, until it is reaching null (it is the smallest of all) or find its right position
    if (_pred(new_node->value, value))
    {
        // if nothing is on the left, go connect the node pointers, n is left
        if (!left)
        {
            left = new_node;
            new_node->parent = this;
            if (parent == nullptr) { return; } // if inserting at root
            correct_color_insert("left");
        }
        // if something is on the left, go compare recurssively
        else { left->insert_node(new_node, _pred); }
    }
    // insert node to the farthest right if it is smaller, until it is reaching null (it is the largest of all) or find its right position
    else if (_pred(value, new_node->value))
    {
        // if nothing is on the right, go connect the node pointers, n is right
        if (!right)
        {
            right = new_node;
            new_node->parent = this;
            if (parent == nullptr) { return; } // if inserting at root
            correct_color_insert("right");
        }
        // if something is on the right, go compare recurssively
        else { right->insert_node(new_node, _pred); }
    }
    else // this means the value inserted is repeated, so do nothing
    {
        delete new_node;
        new_node = NULL;
    }
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::iterator& rbt<T, compare_type>::iterator::operator++()
{
    // simply return the next node found by the helper function
    this_node = find_next_node();
    return *this;
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::const_iterator& rbt<T, compare_type>::const_iterator::operator++()
{
    // simply return the next node found by the helper function
    this_node = find_next_node();
    return *this;
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::iterator rbt<T, compare_type>::iterator::operator++(int)
{
    // use the prefix to define postfix
    iterator copy(*this);
    ++(*this);
    return copy;
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::const_iterator rbt<T, compare_type>::const_iterator::operator++(int)
{
    // use the prefix to define postfix
    const_iterator copy(*this);
    ++(*this);
    return copy;
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::iterator& rbt<T, compare_type>::iterator::operator--()
{
    // simply return the previous node found by the helper function
    this_node = find_previous_node();
    return *this;
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::const_iterator& rbt<T, compare_type>::const_iterator::operator--()
{
    // simply return the previous node found by the helper function
    this_node = find_previous_node();
    return *this;
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::iterator rbt<T, compare_type>::iterator::operator--(int)
{
    // use the prefix to define postfix
    iterator copy(*this);
    --(*this);
    return copy;
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::const_iterator rbt<T, compare_type>::const_iterator::operator--(int)
{
    // use the prefix to define postfix
    const_iterator copy(*this);
    --(*this);
    return copy;
}

template< typename T, typename compare_type >
const T& rbt<T, compare_type>::iterator::operator*() const
{
    // return value if found, else return null
    if (this_node != nullptr) { return this_node->value; }
    else { throw; } // throw an error if iterator point to null node
}

template< typename T, typename compare_type >
const T& rbt<T, compare_type>::const_iterator::operator*() const
{
    // return value if found, else return null
    if (this_node != nullptr) { return this_node->value; }
    else { throw; } // throw an error if iterator point to null node
}

template< typename T, typename compare_type >
const T* rbt<T, compare_type>::iterator::operator->() const { return & (this_node->value); }

template< typename T, typename compare_type >
const T* rbt<T, compare_type>::const_iterator::operator->() const { return & (this_node->value); }

template< typename T, typename compare_type >
bool rbt< T, compare_type >::iterator::operator==(iterator other) const
{
    // check if two iterators are equal, note if both being null is considered NOT equal (return false)
    if (this_node && other.this_node) { return this_node->get_node_val() == other.this_node->get_node_val(); }
    // both nullptr is assumed to be equal
    else if (this_node == nullptr && other.this_node == nullptr) { return true; }
    // otherwise return false
    else { return false; }
}

template< typename T, typename compare_type >
bool rbt< T, compare_type >::const_iterator::operator==(const_iterator other) const
{
    // check if two iterators are equal, note if both being null is considered NOT equal (return false)
    if (this_node && other.this_node) { return this_node->get_node_val() == other.this_node->get_node_val(); }
    // both nullptr is assumed to be equal
    else if (this_node == nullptr && other.this_node == nullptr) { return true; }
    // otherwise return false
    else { return false; }
}

template< typename T, typename compare_type >
bool rbt< T, compare_type >::iterator::operator!=(iterator other) const
{
    // check if two iterators are not equal, note if both being null is considered NOT equal (return true)
    if (this_node && other.this_node) { return this_node->get_node_val() != other.this_node->get_node_val(); }
    // both nullptr are false for "not equal"
    else if (this_node == nullptr && other.this_node == nullptr) { return false; }
    // other wise return true
    else { return true; }
}

template< typename T, typename compare_type >
bool rbt< T, compare_type >::const_iterator::operator!=(const_iterator other) const
{
    // check if two iterators are not equal, note if both being null is considered NOT equal (return true)
    if (this_node && other.this_node) { return this_node->get_node_val() != other.this_node->get_node_val(); }
    // both nullptr are false for "not equal"
    else if (this_node == nullptr && other.this_node == nullptr) { return false; }
    // other wise return true
    else { return true; }
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::iterator rbt<T, compare_type>::begin()
{
    // always go the left most child and return its iterator
    if (root == nullptr) { return iterator(nullptr, this); }
    node *n = root;
    while (n->left != nullptr) { n = n->left; }
    return iterator(n, this);
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::const_iterator rbt<T, compare_type>::begin() const
{
    // always go the left most child and return its iterator
    if (root == nullptr) { return const_iterator(nullptr, this); }
    node *n = root;
    while (n->left != nullptr) { n = n->left; }
    return const_iterator(n, this);
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::iterator rbt<T, compare_type>::end()
{
    // always return the null iterator since it is the past-the-end position
    return iterator(nullptr, this);
}

template< typename T, typename compare_type >
typename rbt<T, compare_type>::const_iterator rbt<T, compare_type>::end() const
{
    // always return the null iterator since it is the past-the-end position
    return const_iterator(nullptr, this);
}

template< typename T, typename compare_type >
void rbt<T, compare_type>::iterator::print_iter_node(const std::string& depth_padding)
{
    const std::string node_position = this_node->find_node_position();
    const std::string color_abrev = (this_node->color == "red") ? "(r)" : "(b)";
    if (node_position == "root") { std::cout << "\n" << depth_padding << "-" << this_node->get_node_val() << color_abrev << "\n"; }
    else if (node_position == "left") { std::cout << "\n" << depth_padding << "\\" << this_node->get_node_val() << color_abrev << "\n"; }
    else { std::cout << "\n" << depth_padding << "/" << this_node->get_node_val() << color_abrev << "\n"; }
}

template< typename T, typename compare_type >
void rbt<T, compare_type>::const_iterator::print_iter_node(const std::string& depth_padding) const
{
    const std::string node_position = this_node->find_node_position();
    const std::string color_abrev = (this_node->color == "red") ? "(r)" : "(b)";
    if (node_position == "root") { std::cout << "\n" << depth_padding << "-" << this_node->get_node_val() << color_abrev << "\n"; } // what to print for root
    else if (node_position == "left") { std::cout << "\n" << depth_padding << "\\" << this_node->get_node_val() << color_abrev << "\n"; } // what to print for left childs
    else { std::cout << "\n" << depth_padding << "/" << this_node->get_node_val() << color_abrev << "\n"; } // what to print for right child
}

template< typename T, typename compare_type >
void rbt<T, compare_type>::insert(const T& other)
{
    // if not exist a root, get new root
    if (root == NULL)
    {
        root = new node(other, "black"); // root is always black
        ++tree_size;
    }
    // otherwise, recursively insert from the root
    else
    {
        node *new_node = new node(other, "red"); // if not root, always initialize to red
        root->insert_node(new_node, pred);
        ++tree_size;
    }
    root->color = "black";
}

template< typename T, typename compare_type >
void rbt<T, compare_type>::insert(T&& other)
{
    // if not exist a root, get new root
    if (root == NULL)
    {
        root = new node(std::move(other), "black");
        ++tree_size;
    }
    //otherwise, recursively insert from the root
    else
    {
        node *new_node = new node(std::move(other), "red");
        root->insert_node(new_node, pred);
        ++tree_size;
    }
    root->color = "black";
}

template< typename T, typename compare_type >
void rbt<T, compare_type>::print()
{
    iterator curr = largest();
    while (curr.this_node != nullptr)
    {
        int curr_depth = node_depth(root, curr.this_node, 0);
        const std::string padding_per_depth = "          ";
        std::string curr_padding = "";
        for (int i = 0; i < curr_depth; ++i) { curr_padding += padding_per_depth; }
        curr.print_iter_node(curr_padding);
        --curr;
    }
}

template< typename T, typename compare_type >
void rbt<T, compare_type>::erase(rbt<T, compare_type>::iterator iter)
{
    // if iterator not belong to this tree or points to a null node, then do nothing
    if (iter.container != this || iter.this_node == nullptr) { return; }
    
    // now the iterator to delete must exist within the correct tree with a valid value, it cannot point to a calue doesn't exist in this tree
    node *curr = iter.this_node;
    std::string node_position = curr->find_node_position();

    node* sibling = curr->find_node_sibling();
    std::string sibling_color = "black";
    if (sibling != nullptr && sibling->color == "red") { sibling_color = "red"; }

    
    //if trying to delete node with no left nor right child, simply points its parent's corresponding to nullptr
    if (curr->left == nullptr && curr->right == nullptr)
    {
        // color corrections
        if (curr->color == "red") { } // if itself is red and has no children
        if (curr->color == "black" && node_position != "root") // if itself is black and has no children and is not root
        {
            curr->color = "double-black"; // first set to double-black, to hint need to go up to fix depth
            // has a red sibling and is left child of parent
            if (sibling_color == "red" && node_position == "left") { curr->parent->left_rotate(); }
            // has a red sibling and is right child of parent
            else if (sibling_color == "red" && node_position == "right") { curr->parent->right_rotate(); }
        }
        --tree_size; // size corrections
        
        // connection corrections
        if (node_position == "left") { curr->parent->left = nullptr; }
        else if (node_position == "right") { curr->parent->right = nullptr; }
        else { } // else, node_positon == 0, only has a root and deleting the root, nothing really to do
        curr->parent = nullptr;
        iter.container = nullptr;
        delete curr;
        root->color = "black";
    } // end of both left and right are empty
    
    // if tring to delete node with right child only, connect its right child to it's parent's corresponding child. If deleting the root, make the right child the new root.
    else if (curr->left == nullptr && curr->right != nullptr)
    {
        curr->correct_color_erase(); // color correction
        --tree_size; // size correction
        // connection correstions
        if (node_position == "root") // deleting root node
        {
            curr->right->parent = nullptr;
            root = curr->right;
        }
        else if (node_position == "left") // deleting parent's left child
        {
            curr->parent->left = curr->right;
            curr->right->parent = curr->parent;
        }
        else // deleting parent's right child
        {
            curr->parent->right = curr->right;
            curr->right->parent = curr->parent;
        }
        curr->parent = nullptr;
        curr->right = nullptr;
        iter.container = nullptr;
        delete curr;
        root->color = "black";
    } // end of left empty, right non-empty
    
    // if tring to delete node with left child only, connect its left child to it's parent's corresponding child. If deleting the root, make the left child the new root.
    else if (curr->left != nullptr && curr->right == nullptr)
    {
        curr->correct_color_erase(); // color correction
        --tree_size; // size correction
        if (node_position == "root") // deleting root node
        {
            curr->left->parent = nullptr;
            root = curr->left;
        }
        else if (node_position == "left") // deleting parent's left child
        {
            curr->parent->left = curr->right;
            curr->right->parent = curr->parent;
        }
        else // deleting parent's right child
        {
            curr->parent->right = curr->right;
            curr->right->parent = curr->parent;
        }
        curr->parent = nullptr;
        curr->left = nullptr;
        iter.container = nullptr;
        delete curr;
        root->color = "black";
    } // end of left non-empty, right is empty
    
    // get both left and right child, find the next larger node, put its value to this node, and call erase recurssively on the next larger node
    else
    {
        node* smallest_next_node = iter.find_next_node();
        iterator smallest_next_iter = iterator(smallest_next_node, this);
        iter.this_node->value = smallest_next_node->value;
        erase(smallest_next_iter);
    } // end of having both left and right child
}

#endif /* rbt_h */
