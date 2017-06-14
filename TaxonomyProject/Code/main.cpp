/* EDAMI Taxonomy project
**Autor: Ruben Romero Hernandez
**Date: 14/06/2017
**Description:
**This program was create to modify data sets applying taxonomies. It gives a small interface and certain flexibility to
**modified the taxonomy.
**The code is not optimized, so mostly the class tree and node are easy to improve and organize in a better way. But those were
**not the aim of that practice so I did not spend more time with them. This code could be a good base to create a good taxonomy
**program if the class tree is improve, and someone create a new class taxonomy with all the functionality to modify deleting,
**adding, pruning and a interface using it in a easy way to use for the users.
**Efficiency. The bottleneck of this program is the method search of tree. The final program should allow create trees which
**contain a class T in several nodes (as a node with several parent). To do that it would be necessary to use a unique key for each node
**I mean a struck{ int key; T t }; or to overwrite the methods of tree in taxonomy adapting them to the new structure.
**I consider that task is further to my purpose and I let it for another people to do it.
*/

#include <algorithm>
#include <cassert>
#include <queue>
#include <stack>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <set>

using namespace std;


char DELIMITER = '*';

    namespace operatorstring{

      /*Overwrite operator << for string
      read till ' ' or - leaving delimiter in the ostream
    */
      istream& operator>> (istream& is, string& str){
      char c;
        while(is.peek() != ' ' && is.peek() != DELIMITER && is.peek() != '\t'){
          is.get(c);
          str.push_back(c);
        }
    }
  }


     /**
    TDA Node
    @doc Definición : Una instancia \a a del tipo de dato abstracto ArbolBinario sobre un
    dominio T se puede construir como:
      - Un objeto vacío si no contiene ningun elemento
      - Un arbol que contiene un elemento destacad la raíz con un valor e en el dominio T
         denominado etiqueta y dos subarboles (Ti:subarbol izquierdo, y Td:subarbol derecho) del
         T.D.A Arbolbinario sobre T. Se establece una relación padre-hijo entre cada nodo y los
         nodos raíz de los subarboles ( si existen) que cuelgan de el. Lo denotamos como
         {e,{Ti},{Td}}
      Para poder usar el TDA ArbolBinario se debe incluir ArbolBinario.h
      Son mutables. El espacio requerido para el almacenamiento es O(n) siendo n el numero de nodos.
  */

   //_______________________________________CLASS NODE _______________________________________________________
  template <class T>
    class Node{
          public:
            // parent
            Node<T> *parent;
            // first, last child
            Node<T> *first_child;
            // brothers
            Node<T> *prev_sibling, *next_sibling;
            // the data of this node
            T data;

            Node();
            Node(const T&);
            int numNodos();
            int getLevel();
            Node<T>* getChildren(int pos);
            Node<T>* getChildren(const T &t);
            Node<T>* getSubTree(const T &t);
            Node<T>* search(const T &t);
            bool deleteNodeChildren(int position );
            bool deleteNodeChildrenWith( const T &t );
            bool deleteSubTree(const T &t);
            Node<T>* insert( int position , Node<T>* node);
            void writePerLevel(ostream &os);
            void writePreOrder(ostream &os);
            void readPerLevel(istream &is);
            void readPreOrder(istream &is);
      };


      template <class T>
      Node<T>::Node(){
        this->parent=NULL;
        this->first_child=NULL;
        this->prev_sibling=NULL;
        this->next_sibling=NULL;
      }


      template <class T>
      Node<T>::Node(const T& v){
        this->parent=NULL;
        this->first_child=NULL;
        this->prev_sibling=NULL;
        this->next_sibling=NULL;
        data=v;
      }

      /**
        @brief return num nodos of Node
      */
      template <class T>
      int Node<T>::numNodos(){
        int res=1;
        Node<T> *tmp=this->first_child;
        while(tmp != NULL){
            res+= tmp->numNodos();
            tmp=tmp->next_sibling;
        }
        return res;
      }


      template <class T>
      int Node<T>::getLevel(){
        int level= 0;
        Node<T> *tmp = this;
        while(tmp->parent != NULL){
          tmp= tmp->parent;
          level++;
        }
        return level;
      }

       /**
        @brief prune a subtree of a node
        @return return root of new tree
      */
      template <class T>
      Node<T>* Node<T>::getSubTree(const T &t){
          Node<T> *aux = this->search(t);
          if(aux != NULL){
            if( aux->next_sibling != NULL)
                aux->next_sibling->prev_sibling = aux->prev_sibling;
            if( aux->prev_sibling != NULL)
                aux->prev_sibling->next_sibling = aux->next_sibling;
            if( aux->parent != NULL && aux->parent->first_child == aux)
                if(aux->next_sibling != NULL)
                    aux->parent->first_child= aux->next_sibling;
                else
                    aux->parent->first_child = NULL;
          }
          aux->parent = aux->next_sibling = aux->prev_sibling = NULL;
          return aux;

      }
      /**
        @brief Delete a node that contain t and all tis descendants
        @return return a true if was successful
      */
      template <class T>
      bool Node<T>::deleteSubTree(const T &t){
          Node<T> *aux , *aux2 = this->getSubTree(t);
          aux = aux2;

          if(aux == NULL)
            return false;

          stack<Node<T> *> mystack;
          mystack.push(aux);

          // If aux is not root we need to separate aux first
          while(mystack.empty() == false){
              aux = mystack.top();
              // Case 0: there is no next_sibling or first child delete and end
              if(aux->first_child == NULL && aux->next_sibling == NULL){
                if(aux->prev_sibling != NULL)
                    aux->prev_sibling->next_sibling = NULL;

                if(aux->parent != NULL && aux->parent->first_child == aux)
                    aux->parent->first_child = NULL;

                mystack.pop();
                delete aux;

              }
              // Case 1: there is first child, uses stuck to save the father and check the children.
              else if(aux->first_child != NULL){
                aux=aux->first_child; // call the same method for the children of aux.

                do{
                    mystack.push(aux);
                    aux = aux->next_sibling;
                }while(aux != NULL);
              }
          }
        return true;
      }


      /**
        @brief Find the children in the position pos
        @return return a pointer to the node if it finde it, otherwise return NULL
      */
      template <class T>
      Node<T>* Node<T>::getChildren(int pos){
        int i ;
        Node<T> *tmp=NULL;
        //Case 0 : it has no childrens
        tmp = this->first_child;
        if(tmp==NULL || pos < 0){
          return NULL;
        }
        //Case 1: pos == 0
        else if( pos == 0){
          return tmp;
        }
        else{
          i=0;
          while(tmp->next_sibling != NULL && i < pos ){
              i++;
              tmp = tmp->next_sibling;
              if( pos == i){
                return tmp;
              }
          }
        }
        return NULL;
      }
      /**
        @brief Find the children in the position pos
        @return return a pointer to the node if it finde it, otherwise return NULL
      */
      template <class T>
      Node<T>* Node<T>::getChildren(const T &t){
        Node<T> *tmp;
        tmp=this->first_child;
        //Case 0: there are no childs
        if( tmp == NULL){
          return NULL;
        }
        //Otherwise
        else{
          while(tmp->data != t && tmp->next_sibling != NULL){
            tmp= tmp->next_sibling;
          }
        }

        if(tmp->data == t){
          return tmp;
        }
        else{
          return NULL;
        }
      }

      /**
        @brief Search the T data in the tree. O(n)
        @param data: const T data to search for
        @ret: return Node<T>*  to the first nodo that contain data.
      */
      template <class T>
      Node<T>* Node<T>::search(const T& data){
        Node<T> * tmp,* tmp2=NULL;
        //Case 0: is root
        if(this->data == data){
          return this;
        }
        //Case 1: it is one of this children
        tmp = this->first_child;
        while(tmp != NULL && tmp2 == NULL){
          tmp2 = tmp->search(data);
          tmp = tmp->next_sibling;
        }
        return tmp2;
      }
      /**
      @brief deleteNode childrens with value T = t. Find the node children or grandchildren or ...  with T = t and delete it
      @param t: delete children with this value
      @return: true if it delete a node, otherwise false
      */
      template <class T>
      bool Node<T>::deleteNodeChildrenWith(const T &t){
        Node<T> *tmp;
        int pos;
        bool find = false;

        //Case 0: Find the children with T = t
        tmp = this->first_child;
        pos=0;
        while(tmp != NULL && !find){
          if( tmp->data == t ){
            find=true;
            this->deleteNodeChildren(pos);
          }
          tmp = tmp->next_sibling;
          pos++;
        }

        return find;
      }
      /**
      @brief deleteNode children in the pos position
      @param position: position of the children to delete
      @return: node a pointer to this
      */
      template <class T>
      bool Node<T>::deleteNodeChildren(int position){
        int pos;
        Node<T> *tmp,*tmp2;
        //Case 0: it has no children
        if(this->first_child==NULL || position < -1){
          return false;
        }
        //Case 1: just 1 child
        tmp = this->first_child;
        if( tmp->next_sibling == NULL ){
            tmp->parent->first_child = NULL;
            tmp2 = tmp->first_child;
            //Insert childrens of delete node.
            while(tmp2 != NULL){
                this->insert(-1,tmp2);
                tmp2 = tmp2->next_sibling;
            }
            tmp->parent = tmp->first_child = tmp->next_sibling = tmp->prev_sibling = NULL;
            delete tmp;

        }
        //Case 2: pos == 0 delete first child and there is at least 2 child
        else if( position == 0){
            tmp->parent->first_child = tmp->next_sibling;
            tmp->next_sibling->prev_sibling = NULL;
            tmp2 = tmp->first_child;
            //Insert childrens of delete node.
            while(tmp2 != NULL){
                this->insert(-1,tmp2);
                tmp2 = tmp2->next_sibling;
            }
            tmp->parent = tmp->first_child = tmp->next_sibling = tmp->prev_sibling = NULL;
            delete tmp;
        }
        //Case 3: pos == -1 and there is at least 2  children nodes. Delete last one
        else if( position == -1){
           while( tmp->next_sibling != NULL)
                tmp = tmp->next_sibling;

           tmp->prev_sibling->next_sibling = NULL;
           //Insert childrens of delete node.
            while(tmp2 != NULL){
                this->insert(-1,tmp2);
                tmp2 = tmp2->next_sibling;
            }
            tmp->parent = tmp->first_child = tmp->next_sibling = tmp->prev_sibling = NULL;
            delete tmp;

        }
        // Case 4: pos > 0 and more than 1 children node
        else if( position > 0 ){
            pos = 0;
            while( tmp->next_sibling != NULL && pos < position){
                tmp = tmp->next_sibling;
                ++pos;
            }
            if( tmp->next_sibling == NULL){ // we are at the last pos
                tmp->prev_sibling->next_sibling = NULL;
            }
            else{ // we are in the middle
                tmp->prev_sibling->next_sibling = tmp->next_sibling;
                tmp->next_sibling->prev_sibling = tmp->prev_sibling;
            }
             //Insert childrens of delete node.
             tmp2 = tmp->first_child;
            while(tmp2 != NULL){
                this->insert(-1,tmp2);
                tmp2 = tmp2->next_sibling;
            }
            tmp->parent = tmp->first_child = tmp->next_sibling = tmp->prev_sibling = NULL;
            delete tmp;
        }


        return true;
      }



      /**
      @brief Insert a new nodo at node parent
      @param position: position in which the child will be inseted
      @param node: Node<T>*  to insert
      @return: node that have just been inserted
      */
      template <class T>
      Node<T>* Node<T>::insert( int position , Node<T>* node){
        Node<T>* tmp;
        int i = 0;

        if ( node == NULL){
            return NULL;
        }
        node->parent = this; //Address of this right
        //Case 0: this has no childrens
        if(first_child==NULL){
          this->first_child = node;
          node->next_sibling=node->prev_sibling = NULL;
          return node;
        }
        //Case 1: no specific position
        else if (position == -1){
          //If is not specified position we will add the node at the last position and there are children, find the last one.
          tmp= this->first_child;
          while (tmp->next_sibling != NULL) {
            tmp = tmp->next_sibling;
          }
          tmp->next_sibling = node;
          node->prev_sibling = tmp;
          node->next_sibling = NULL;
          return node;
        }

        //Case 2: Insert in a specific position.
        else {
          //get nth child of parent
          tmp = this->first_child;
          // Case 2.1 : pos 0
          if(position == 0){
            this->first_child=node;
            node->prev_sibling=NULL;
            node->next_sibling=tmp;
            tmp->prev_sibling=node;
            return node;
          }
          //Case 2.2 : pos != 0
          do{
            if(i==position || tmp->next_sibling == NULL){ // insert in this position
               tmp->prev_sibling->next_sibling=node;
               node->prev_sibling=tmp->prev_sibling;
               node->next_sibling=tmp;
               tmp->prev_sibling=node;
               return node;
            }

          tmp=tmp->next_sibling;
          i++;
          }while(tmp->next_sibling != NULL);
        }

        return NULL;
      }



      /**
      @brief Write in the buffer the tree per level: Ex
        level 0: 1
        level 1: 1->( 2, 3, 4, 5 )
        level 2: 2->( 6, 7, 8 ), 3->( ), 4->( ), 5->( 9, 10)
      */
      template <class T>
      void Node<T>::writePerLevel(ostream &os){
        int level=1;
        Node<T> *tmp, *tmp2;
        queue<Node<T>* > myQueue;

        myQueue.push(this);

        os << "level 0"<< ": "<<DELIMITER << this->data << DELIMITER<<endl;
        os << "level " << level << ": ";

        while(myQueue.empty() == false){
          tmp=myQueue.front(); // oldest element
          myQueue.pop();
          if( tmp->first_child != NULL){
              os << DELIMITER << tmp->data << DELIMITER;
              os << "->( " ; // next level
              tmp2= tmp->first_child;
              do{
                 os << DELIMITER ;
                 os << tmp2->data ;
                 os << DELIMITER ;
                 myQueue.push(tmp2);
                 tmp2=tmp2->next_sibling;
                 if(tmp2!=NULL)
                   os << ", ";
              }while(tmp2 != NULL);
            os << " )" ;
            //Check if we go on at the same level write , change level write \nlevel n: or end the program

            //Get next element with childs
            tmp2 = myQueue.front();
            if ( tmp2 != NULL){
                while( myQueue.empty() == false && tmp2->first_child == NULL ){
                    myQueue.pop();
                    tmp2 = myQueue.front();
                }
            }
            if(myQueue.empty() == false){
              if(tmp->getLevel() == tmp2->getLevel())
                os << ",  ";

              else{
                os << "\n";
                level++;
                os << "level " << level << ": ";
               }

            }
            else
                os << endl;

          }
        }

      }

      /**
      @brief Write in buffer os the tree in prerorder. Ex: 1 ( 2 ( 6,7 ,8 ), 3, 4, 5 ( 9, 10 ),)
      */
      template <class T>
      void Node<T>::writePreOrder(ostream &os){
        Node<T> * tmp=this;
        os << DELIMITER ;
        os << tmp->data ;
        os << DELIMITER ;
        tmp=this->first_child;
        if(tmp!=NULL){
          os << " ( " ;
          tmp->writePreOrder(os);
          while(tmp->next_sibling!=NULL){
            os << ", " ;
            tmp= tmp->next_sibling;
            tmp->writePreOrder(os);
          }

          if(this->parent != NULL)
            os << " )" ;
          else
             os << " ) ";
        }
      }

      /**
      @brief Read of the buffer the tree per level: Ex
          $
         -1- '\n'
         1->( -2-, -3-, -4-, -5- ) '\n'
         2->( -6-, -7-, -8- ), 3->( ), 4->( ), 5->( -9-, -10-) '\n'
         6->( ), 7->( ), 8->( ), 9->( ), 10->( )
         legend:
         ( means start childs
         , means there is another child more
         - - to acotate datas
      */
      template <class T>
      void Node<T>::readPerLevel(istream &is){
        using namespace operatorstring;  // _______________________________ I have no idea if this is appropiate but it works
        bool readNextFather = true;
        T *t=NULL;
        char c;
        Node<T> *tmp, *tmp2;
        queue< Node<T> * > myQueue;

         /* EX:
                $
                level 0:-1-
                level 1:1->( 2, 3 )
                level 2:2->( 4, 5 ), 3->( 6 )
                level 3:4->( 7 ), 5->( ), 6->( 8 )
                level 4:7->( ), 8->( )

                $
                level 0:*sofware-enginering*
         Q       level 1:*sofware-enginering*->( *programming*, *Apps* )
                */

        //Read first line
        is.ignore(1024,'$');
        is.ignore(1024,':');
        t = new T;
        is.get();
        is >> *t;
        this->data = *t;
        is.ignore(1024,':');

        tmp = this;
        myQueue.push(tmp);
        while( is && myQueue.empty() == false){
          c = is.get();

          // Case 1:
          if( c == DELIMITER ){
            delete t;
            t = new T;
            is >> *t;
            is.get(); // Delimiter

            do{

              tmp2 = myQueue.front();
              myQueue.pop();
            }while(tmp2->data != *t && myQueue.empty() == false );

            //Read childrens
            while( c != ')'){

              c = is.get();
              if( c == DELIMITER){
                delete t;
                t = new T;
                is >> *t;
                is.get(); // Delimiter
                tmp = new Node<T>();
                tmp->data = *t;
                tmp2->insert(-1,tmp); // add children
                myQueue.push(tmp); // insert in the queue
              }
            }
          }
        }
      }

   //_______________________________________FIN  NODE ______________________________________________________________________



//____________________________________    TREE    ______________________________________________

template <class T>
class Tree {
  private:


    Node<T> *root; // roor of N-ary tree


  public:
    Tree();
    Tree(T &t);
    Tree(Node<T>* node);
    ~Tree();
    // ops
    int numNodos(){ if( root != NULL) return root->numNodos(); else return 0;}
    void createRoot(T &t);
    void clear(Node<T>* node);
    bool deleteSubTree(const T &t);
    Node<T>* getRoot()const;
    Tree<T>* PruneSubTree(const T &t);
    Node<T>* insert(Node<T>* , int, Node<T>* );
    Node<T>* search(const T&);
    bool remove(const T& data);
    void writePerLevel(ostream &os);
    void readPerLevel(istream &is);
    void writePreOrder(ostream &os);
};



// constructor
template <class T>
Tree<T>::Tree() {
	root = new Node<T>();
}
// constructor
template <class T>
Tree<T>::Tree(T &t) {
  this->createRoot(t);
}
// constructor
template <class T>
Tree<T>::Tree(Node<T> * node) {
  root = node;
}

// destructor
template <class T>
Tree<T>::~Tree() {
  //clear(root);
}


/**
  @brief create a root
 */
template <class T>
void Tree<T>::createRoot(T &t){
      if( root != NULL){
        clear(root);
      }
      root = new Node<T>(t);
}
/**
  @brief return the root node
 */
template <class T>
Node<T>* Tree<T>::getRoot()const{
  return this->root;
}

 /**
    @brief prune a subtree of a node
    @return return a new tree
  */
  template <class T>
  Tree<T>* Tree<T>::PruneSubTree(const T &t){
      Tree<T>* result=NULL;

      if(this->root != NULL){
        result->createRoot(this->root->getSubTree(t));
      }
      return result;

  }
/**
  @brief Delete the subtree and if it is necessary link their sublings
*/
template <class T>
void Tree<T>::clear(Node<T> * node){
  //Case 0: the tree is empty
  assert(this->root != NULL);
  this->deleteSubTree();
}

/**
  @brief delete a subtree
  @return return true if the operation was successful
**/
template <class T>
bool Tree<T>::deleteSubTree(const T &t){
  Node<T> *aux, *aux2;
  aux = this->root;

  if(aux == NULL )
        return false;

    return aux->deleteSubTree(t);

}

/**
  @brief Insert a new nodo at node parent
  @param parent: Node<T>* that point to the node that will get the new child
  @param position: position in wich the child will be inseted
  @param node: Node<T>*  to insert
  @return node that have just been inserted
*/
template <class T>
Node<T>* Tree<T>::insert(Node<T>* parent, int position , Node<T>* node) {
  Node<T>* tmp;
  int i = 0;

  // Case 0: node == NULL
  assert(node != NULL || root != NULL);

  // Case 1: parent is null
  if (parent == NULL) {
      parent = root;

  }
  // Case 2: parent is a node, but we need to check it node is from this tree
  else{
  	if(this->search(node->data) == NULL)
  		return NULL;
  }


  return parent->insert(position,node);
}


/**
  @brief Search the T data in the tree. O(n)
  @param data: const T data to search for
  @ret: return Node<T>*  to the first nodo that contain data.
*/
template <class T>
Node<T>* Tree<T>::search(const T& data){
    return this->root->search(data);
}

/**
  @brief Remove node with data
  @param data: data
  @ret: true if it removed something, false otherwise
*/
template <class T>
bool Tree<T>::remove(const T& data){
  Node<T> *aux;
  bool res = false;

  if (root == NULL)
    return false;

  //Case 0, it is in root
  if( root->data == data){
     //If it happend we should create diferent subtrees, one for each children of root
  }
  //Case 1:
  aux = this->search(data);
  if( aux != NULL){
    aux = aux->parent;
    if( aux != NULL)
      res = aux->deleteNodeChildrenWith(data);
  }

  return res;
}

/**
@brief Read of the buffer the tree per level: Ex
    $
   -1- '\n'
   1->( -2-, -3-, -4-, -5- ) '\n'
   2->( -6-, -7-, -8- ), 3->( ), 4->( ), 5->( -9-, -10-) '\n'
   6->( ), 7->( ), 8->( ), 9->( ), 10->( )
   legend:
   ( means start childs
   , means there is another child more
   - - to acotate datas
*/
template <class T>
void Tree<T>::readPerLevel(istream &is){
  if(root != NULL){
    root->readPerLevel(is);
  }
}

/**
@brief Write in the buffer the tree per level: Ex
  level 0: 1
  level 1: 1->( 2, 3, 4, 5 )
  level 2: 2->( 6, 7, 8 ), 3->( ), 4->( ), 5->( 9, 10)
*/
template <class T>
void Tree<T>::writePerLevel(ostream &os){
  if( root != NULL)
    root->writePerLevel(os);

}

template <class T>
void Tree<T>::writePreOrder(ostream &os){
  if( root != NULL)
      root->writePreOrder(os);
}









struct RowSet{
        int sequenceID;
        int eventID;
        int size;
        set<string> items;
    };

class DataSet{
  private:
    vector<string> colnames;
    vector<RowSet> datas;
    set<string> items;
  public:


    DataSet(){
    }

    DataSet(istream &is){
      readSet(is);
    }

    void readSet(istream &is){
      int i ;
      RowSet *row;
      string s1, s2;
      size_t pos;
      char delimeter = ' ';
      char c;
      //First read the col
      if( is ){

        do{
          c=is.get(); // read char
          if(c != delimeter && c != '\n'){
            s1.push_back(c); // add to s1
          }
          else{
            colnames.push_back(s1);
            s1.clear();
          }
        }while(c != '\n');
        //// read rest of the set. we know the structure it will follow
        //// sequenceID eventID SIZE              items
        ////      2        1      5  design Webdesign inspiration icons images
        do{
          row = new RowSet;
          is >> row->sequenceID;
          is >> row->eventID;
          is >> row->size;
          //Read items
          do{

            c= is.get();
            //cout << c << endl;

            if(c != delimeter && c != '\n'){
              s1.push_back(c); // add to s1
            }
            else if( !s1.empty()){
              items.insert(s1);
              row->items.insert(s1);
              s1.clear();
            }
          }while(c != '\n' );


          datas.push_back(*row);
          is.peek(); // to test if the end is the next
        }while( !is.eof()  );
      }
    }

    void deleteItems(set<string> &dset){
       vector<RowSet>::iterator it = datas.begin();
       set<string>::iterator it2;
       while( it != datas.end()){
            it2 = dset.begin();
           while(it2 != dset.end()){
                (*it).items.erase((*it2));
                it2++;
           }
        ++it;
       }
    }

    void saveInFile(string name){
      ofstream myfile;
      myfile.open(name.c_str());
      cout << colnames.size() << "   " << datas.size() << endl;
      if(myfile){
        vector<string>::iterator it = colnames.begin();
        while(  it != colnames.end() ){
          myfile << *it;
          myfile << " ";
          ++it;
        }
        myfile << '\n';

        vector<RowSet>::iterator it2 = datas.begin();
        while( it2 != datas.end()){
          myfile << (*it2).sequenceID;
          myfile << " ";
          myfile << (*it2).eventID;
          myfile << " ";
          myfile << (*it2).size;
          myfile << " ";
          set<string>::iterator it3 = (*it2).items.begin();
          while( it3 != (*it2).items.end()){
            myfile << *it3;
            myfile << " " ;
            ++it3;
          }
          myfile << '\n';
          ++it2;
        }


      }
    }
    /* It will read the vector of RowSet datas and adding information */
    void applyTaxonomy( Tree<string> taxonomy){
        Node<string> *node;
        vector<RowSet>::iterator itdatas = datas.begin();
        set<string>:: iterator itrows, itrows2;
        pair< set<string>::iterator , bool > p;

        //Iter by datas
        while( itdatas != datas.end()){
          //Iter by each string of rowSet
          itrows = (*itdatas).items.begin();

          while( itrows != (*itdatas).items.end()){
            p.second = false;
            node = taxonomy.search( *itrows);//log(n)
            if(node != NULL && node->parent != NULL){ // It was found so insert new element in that rowSet
              // Insert at the end
              p = (*itdatas).items.insert(node->parent->data); // if it doen't insert it return a it to the same elemenet
              if( p.second ){
                itrows = (*itdatas).items.begin();
              }
            }
            if(p.second == false)
             ++itrows;
          }
          (*itdatas).size = (*itdatas).items.size();
          ++itdatas;
        }
    }
};


void modififingTaxonomy(Tree<string> mytree){
  string s;
  vector<string> toDelete;
  Node<string> *aux, *aux2;
  int selection;
  do{
    mytree.writePerLevel(cout);
    cout << endl;
    cout << "Select the action you want to carry out:\n\t1-Remove elements.\n\t2-Exit.\n" << endl;
    cin >> selection;
    switch (selection){
      case 1:
        do{
           cout << "Introduce the label of a element or * to end the action: " ;
           cin >> s;
           Node<string> *node = mytree.search(s);
           if( s == "*"){
              cout << "Exit action." << endl;
           }
           else if(  node == NULL){
              cout << "The " << s << " label does not belong to the taxonomy. " << endl;
           }
           else{
              cout << "Select the action you want to carry out over label " << s << ":\n\t1-Remove element.\n\t2-Remove label and  all predecessors.\n\t3-Remove all predecessors. \n\t4-Nothing" << endl;
              cin >> selection;
              switch (selection){
                case 1: // Delete the node
                  if(mytree.remove(s) == true){
                    cout << "Removed succesfully" << endl;
                  }
                  else
                    cout << "Impossible to remove" << endl;
                  break;
                case 2: // Delete node and childrens
                  if( mytree.deleteSubTree(s) == true)
                     cout << "Element and predecessors removed successfully" << endl;
                  else
                    cout << "Impossible to remove" << endl;
                  break;
                case 3: // Delete childrens
                    aux = mytree.search(s);
                    if( aux != NULL){
                        aux2=aux->first_child;
                        while(aux2 != NULL){
                            aux = aux2->next_sibling;
                            aux2->deleteSubTree(aux2->data);
                            aux2 = aux;
                        }
                        cout << "Predecessors remove successfully" << endl;
                    }
                    else{
                        cout << "Impossible to remove" << endl;
                    }
                    break;
                default:
                    cout << "Exit action." << endl;
                  break;
              }
           }
           cout << endl;
           mytree.writePerLevel(cout);

        }while( s != "*");
        break;
      default:
        return;
      }
  }while(selection != 2);
}

void deleleteItems(DataSet &dset){
    string selection, items ;
    cout << "Do you want to delete any items of the data set?(yes/no):" << endl;
    cin >> selection;
    if(selection == "yes"){
        cout <<  "Introduce the label of the items that you want to delete: (intro to end)" <<endl;
        cin.ignore(1);
        getline(cin, items, '\n');
    }
    cin.ignore(1);
    istringstream iss(items);
    set<string> tokens;
    copy(istream_iterator<string>(iss),istream_iterator<string>(),inserter(tokens, tokens.begin()));

    dset.deleteItems(tokens);
    cout << endl;
}

void interface(){
    string name, datasetname, selection;
    Tree<string> mytree;
    DataSet dset;
    ifstream myfile;

    // loading data set;
    do{
        cout << " Introduce the name of the file to load the data set " << endl;
        cin >> datasetname;
        myfile.open(datasetname.c_str());
        if(!myfile)
            cout << "Impossible to open the file, try again, or type exit to end" << endl;
    }while(name != "exit" && !myfile);

    dset.readSet(myfile);
    myfile.close();
    deleleteItems(dset);
    // Reading, modifying taxonomies and applying to data set
    do{
        cout << "Do you want to load a taxonomy?(yes/no/other thing to end the program):" << endl;
        cin >> selection ;

        // Reading taxonomies
        if( selection == "yes"){
            do{
                cout << "Introduce the name of the file to load taxonomy:" << endl;
                cin >> name;
                myfile.open (name.c_str());
                if(!myfile)
                    cout << "Impossible to open the file, try again, or type exit to end" << endl;
            }while(name != "exit" && !myfile);

            if(myfile){
                mytree.readPerLevel(myfile);
                // Modifies taxonomies
                cout << "Do you want to modified the taxonomy?(yes/no/other thing to end the program):" << endl;
                cin >> selection ;
                if(selection == "yes"){
                     modififingTaxonomy(mytree);
                }
                else if(selection != "no"){
                    cout << "Ending the program. " << endl;
                    return;
                }
                // applying taxonimies
                cout << "Do you want to apply this taxonomy to the data set " << datasetname <<" ?(yes/no/other thing to end the program)" << endl;
                cin >> selection ;
                if(selection == "yes"){
                     dset.applyTaxonomy(mytree);
                     deleleteItems(dset);
                }
                else if( selection == "no"){
                    selection = "yes"; // to go on with main loop
                }
                else{
                    cout << "Ending the program. " << endl;
                    return;
                }
            }

        }
        else{
                cout << "Do you want to save the data set?(yes/no)" << endl;
                cin >> selection ;
                if(selection == "yes"){
                    cout << "Introduce the filename to save the set." << endl;
                    cin >> name;
                    dset.saveInFile(name);
                    selection = "end";
                }
                else if( selection != "no"){
                    cout << "The resulted data set was not save in any file." << endl;
                    return;
                }
            }
    }while( selection == "yes" );
    cout << "End of program" << endl;
}

int main(){

  interface();

}
