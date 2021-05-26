// Implementing completely fair scheduler in C++
// By Angkit Sarma
#include <iostream>
#include <chrono>
#include <cstdint>
#include <unistd.h>
#include <sys/time.h>
#define Granularity 1                 // 1 nanosecond
#define NICE_0_LOAD 2048.0              

using namespace std;
struct Node
{
  uint64_t data;              //Due to large size

  int pid;
  Node *parent;
  Node *left;
  Node *right;
  int color;
};
typedef Node *NodePtr;
class RedBlackTree
{
private:
  NodePtr root;
  NodePtr TNULL;
  void initializeNULLNode(NodePtr node, NodePtr parent)  // For initilization
  {
    node->data = 0;
    node->pid = 0;
    node->parent = parent;
    node->left = nullptr;
    node->right = nullptr;
    node->color = 0;
  }
  void preOrderHelper(NodePtr node)
  {
    if (node != TNULL)
    {
      cout << node->data << " ";
      preOrderHelper(node->left);
      preOrderHelper(node->right);
    }
  }
  void inOrderHelper(NodePtr node)
  {
    if (node != TNULL)
    {
      inOrderHelper(node->left);
      cout << node->data << " "<<node->pid<<"\n ";
      inOrderHelper(node->right);
    }
  }
  void postOrderHelper(NodePtr node)
  {
    if (node != TNULL)
    {
      postOrderHelper(node->left);
      postOrderHelper(node->right);
      cout << node->data << " ";
    }
  }
  NodePtr searchTreeHelper(NodePtr node, uint64_t key)     // For searching
  {
    if (node == TNULL || key == node->data)
    {
      return node;
    }
    if (key < node->data)
    {
      return searchTreeHelper(node->left, key);
    }
    return searchTreeHelper(node->right, key);
  }
  void deleteFix(NodePtr x)             // For fixing the delete
  {
    NodePtr s;
    while (x != root && x->color == 0)
    {
      if (x == x->parent->left)
      {
        s = x->parent->right;
        if (s->color == 1)
        {
          s->color = 0;
          x->parent->color = 1;
          leftRotate(x->parent);
          s = x->parent->right;
        }
        if (s->left->color == 0 && s->right->color == 0)
        {
          s->color = 1;
          x = x->parent;
        }
        else
        {
          if (s->right->color == 0)
          {
            s->left->color = 0;
            s->color = 1;
            rightRotate(s);
            s = x->parent->right;
          }
          s->color = x->parent->color;
          x->parent->color = 0;
          s->right->color = 0;
          leftRotate(x->parent);
          x = root;
        }
      }
      else
      {
        s = x->parent->left;
        if (s->color == 1)
        {
          s->color = 0;
          x->parent->color = 1;
          rightRotate(x->parent);
          s = x->parent->left;
        }
        if (s->right->color == 0 && s->right->color == 0)
        {
          s->color = 1;
          x = x->parent;
        }
        else
        {
          if (s->left->color == 0)
          {
            s->right->color = 0;
            s->color = 1;
            leftRotate(s);
            s = x->parent->left;
          }
          s->color = x->parent->color;
          x->parent->color = 0;
          s->left->color = 0;
          rightRotate(x->parent);
          x = root;
        }
      }
    }
    x->color = 0;
  }
  void rbTransplant(NodePtr u, NodePtr v)
  {
    if (u->parent == nullptr)
    {
      root = v;
    }
    else if (u == u->parent->left)
    {
      u->parent->left = v;
    }
    else
    {
      u->parent->right = v;
    }
    v->parent = u->parent;
  }
  void deleteNodeHelper(NodePtr node, uint64_t key) // Delete helper
  {
    NodePtr z = TNULL;
    NodePtr x, y;
    while (node != TNULL)
    {
      if (node->data == key)
      {
        z = node;
      }
      if (node->data <= key)
      {
        node = node->right;
      }
      else
      {
        node = node->left;
      }
    }
    if (z == TNULL)
    {
      cout << "Key not found in the tree" << endl;
      return;
    }
    y = z;
    int y_original_color = y->color;
    if (z->left == TNULL)
    {
      x = z->right;
      rbTransplant(z, z->right);
    }
    else if (z->right == TNULL)
    {
      x = z->left;
      rbTransplant(z, z->left);
    }
    else
    {
      y = minimum(z->right);
      y_original_color = y->color;
      x = y->right;
      if (y->parent == z)
      {
        x->parent = y;
      }
      else
      {
        rbTransplant(y, y->right);
        y->right = z->right;
        y->right->parent = y;
      }
      rbTransplant(z, y);
      y->left = z->left;
      y->left->parent = y;
      y->color = z->color;
    }
    delete z;
    if (y_original_color == 0)
    {
      deleteFix(x);
    }
  }
  void insertFix(NodePtr k)
  {
    NodePtr u;
    while (k->parent->color == 1)
    {
      if (k->parent == k->parent->parent->right)
      {
        u = k->parent->parent->left;
        if (u->color == 1)
        {
          u->color = 0;
          k->parent->color = 0;
          k->parent->parent->color = 1;
          k = k->parent->parent;
        }
        else
        {
          if (k == k->parent->left)
          {
            k = k->parent;
            rightRotate(k);
          }
          k->parent->color = 0;
          k->parent->parent->color = 1;
          leftRotate(k->parent->parent);
        }
      }
      else
      {
        u = k->parent->parent->right;
        if (u->color == 1)
        {
          u->color = 0;
          k->parent->color = 0;
          k->parent->parent->color = 1;
          k = k->parent->parent;
        }
        else
        {
          if (k == k->parent->right)
          {
            k = k->parent;
            leftRotate(k);
          }
          k->parent->color = 0;
          k->parent->parent->color = 1;
          rightRotate(k->parent->parent);
        }
      }
      if (k == root)
      {
        break;
      }
    }
    root->color = 0;
  }
  void printHelper(NodePtr root, string indent, bool last)
  {
    if (root != TNULL)
    {
      cout << indent;
      if (last)
      {
        cout << "R----";
        indent += "   ";
      }
      else
      {
        cout << "L----";
        indent += "|  ";
      }
      string sColor = root->color ? "RED" : "BLACK";
      cout << root->data <<  "  pid : "<< root->pid << "  (" << sColor << ")" << endl;
      printHelper(root->left, indent, false);
      printHelper(root->right, indent, true);
    }
  }
public:
  RedBlackTree()
  {
    TNULL = new Node;
    TNULL->color = 0;
    TNULL->left = nullptr;
    TNULL->right = nullptr;
    root = TNULL;
  }
  void preorder()
  {
    preOrderHelper(this->root);
  }
  void inorder()
  {
    inOrderHelper(this->root);
  }
  void postorder()
  {
    postOrderHelper(this->root);
  }
 
  NodePtr searchTree(uint64_t k)               // For searching
  {
    return searchTreeHelper(this->root, k);
  }

  NodePtr minimum(NodePtr node)
  {
    while (node->left != TNULL)
    {
      node = node->left;
    }
    return node;
  }
  NodePtr maximum(NodePtr node)
  {
    while (node->right != TNULL)
    {
      node = node->right;
    }
    return node;
  }
  NodePtr successor(NodePtr x)
  {
    if (x->right != TNULL)
    {
      return minimum(x->right);
    }
    NodePtr y = x->parent;
    while (y != TNULL && x == y->right)
    {
      x = y;
      y = y->parent;
    }
    return y;
  }
  NodePtr predecessor(NodePtr x)
  {
    if (x->left != TNULL)
    {
      return maximum(x->left);
    }
    NodePtr y = x->parent;
    while (y != TNULL && x == y->left)
    {
      x = y;
      y = y->parent;
    }
    return y;
  }
  void leftRotate(NodePtr x)
  {
    NodePtr y = x->right;
    x->right = y->left;
    if (y->left != TNULL)
    {
      y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nullptr)
    {
      this->root = y;
    }
    else if (x == x->parent->left)
    {
      x->parent->left = y;
    }
    else
    {
      x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
  }
  void rightRotate(NodePtr x)
  {
    NodePtr y = x->left;
    x->left = y->right;
    if (y->right != TNULL)
    {
      y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nullptr)
    {
      this->root = y;
    }
    else if (x == x->parent->right)
    {
      x->parent->right = y;
    }
    else
    {
      x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
  }
  void insert(uint64_t key,int pid)        //For insertion
  {
    NodePtr node = new Node;
    node->parent = nullptr;
    node->data = key;
    node->pid = pid;
    node->left = TNULL;
    node->right = TNULL;
    node->color = 1;
    NodePtr y = nullptr;
    NodePtr x = this->root;
    while (x != TNULL)
    {
      y = x;
      if (node->data < x->data)
      {
        x = x->left;
      }
      else
      {
        x = x->right;
      }
    }
    node->parent = y;
    if (y == nullptr)
    {
      root = node;
    }
    else if (node->data < y->data)
    {
      y->left = node;
    }
    else
    {
      y->right = node;
    }
    if (node->parent == nullptr)
    {
      node->color = 0;
      return;
    }
    if (node->parent->parent == nullptr)
    {
      return;
    }
    insertFix(node);
  }
  NodePtr getRoot()           //For getting root
  {
    if((this->root)== TNULL)
    {
      return 0;
    }
    else
      return this->root;
  }
  void deleteNode(uint64_t data)
  {
    deleteNodeHelper(this->root, data);
  }
  void printTree()
  {
    if (root)
    {
      printHelper(this->root, "", true);
    }
  }
  
 
  uint64_t getNext(NodePtr node)
  {
      uint64_t best;
    if(node == NULL)
      return 0;
    if(node->left == nullptr)
    {
            return (node->data);
    }

      // leftmost child in the left subtree is always better than the root
    if (node->left->left == nullptr)
        return (node->data);
    else
        best = getNext(node->left);

     return best;
    
  }

};


void   insertBN(long long int * burst, int * nice)  //Function for insertion of Burst time and Nice Values
{
  int i=0;
   
 	for(i=0;i<5;i++)
	{
	  cout<<"\nPlease enter the BURST TIMES and NICE VALUES (-19 TO 20) respectively";
	  cin>>burst[i];
        
      if(burst[i]<0)  
        {
            cout<<"\n Invalid input! Please enter burst time again!\n";
            cin>>burst[i];
        }    

      cin>>nice[i];

        if(nice[i]<-19 || nice[i]>20)  
        {
            cout<<"\n Invalid input! Please enter nice value again!\n";
            cin>>nice[i];
        }
	  
	  cout<<"\n"<<"Burst"<< "[" <<i << "]"    << ":"<<burst[i]<<"\n";
        
      cout<<"\n"<<"Nice" <<"[" <<i << "]"<< ":" <<nice[i]<< "\n";
	  
	}

}


long long int  totalWeight(long long int * weight) //Function for summation of weights
{
  long int   i=0,sum=0;
  
  for(i=0;i<5;++i)
  {
    sum = sum + weight[i]; 
    
  }
   cout<<"\nTotal weight is "<<sum;
   
  return sum; 
} 

void time_SL( long double * time_slice,int schedule_period,long long int * weight,long long int total_weight)  // Function for timeslice
{
    int i=0;
    for(i=0;i<=5;++i)
    {
       
        time_slice[i] = ( schedule_period * ( (weight[i] * 1.0) / total_weight)); 

        if(time_slice[i] <= Granularity )    // Minimum Granularity
        {
          time_slice[i] = Granularity ;
        }
         
         
    }

}
   
   float  cal(int k,float z)          // Function for calculation of weights
   {    
      if (k == 0 )
      {
         return 1;
    }
     else    
        return  ((z) * cal(k-1,z));
    
   }

  
  void assign_weight(int * nice,long long int * weight)   // Function for assigning weights
  {
        int i=0,k=0;
        
        for( i=0;i<5;++i)
        {
          
          if (nice[i] == 0)
             weight[i] = 2048;
        if(nice[i] >0 )   
      {   
          k=nice[i];
          weight[i] = 2048 * cal(k,(0.8));  //  0.8
                               
       } 
      if(nice[i] < 0)
      {
         k=(-1)*nice[i];
         weight[i] = 2048 * cal(k,(1.25));    // 1.25
         
       } 
             
             
    }


    
  }

  void menu()
  {
            cout << "\n__________________________________________";
            cout << "\n\n  --COMPLETELY FAIR SCHEDULER--";
            cout << "\n          --(Red-Black-Tree)--";
            cout << "\n__________________________________________";
            
           
           
        }
uint64_t timeSinceEpochMillisec()     // Option available for implementation in milliseconds
{
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

  uint64_t delta_exec(long long int* initial,int i)  //implementation in milliseconds
  {
      
   
       long long int s = initial[i];
       
     initial[i] = timeSinceEpochMillisec();
      cout<<"\n old  "<<s;
      cout<<"\n new initial  "<<initial[i];
      
      cout<<"\n fiNAL - INITIAL  "<<(initial[i]- s);
      
    return ((initial[i]- s))  ;
   
 }
 
int main()
{
  RedBlackTree bst;               //Making an object of Red Black Tree
  NodePtr p;                      // Making an object of NodePtr
  
  int info,i=0,input,schedule_period,nice[20];      // Initialization
  long long int total_weight,weight[20],burst[20];;
  uint64_t v_runtime[20],d,initial[20];;
  long double  time_slice[20];

  menu();

for(i=0;i<5;++i)                 // Setting all values to 0
{     
      nice[i]=0;
      v_runtime[i]=0;
      initial[i]=0;
      burst[i]=0;
      weight[i]=0;
      time_slice[i]=0;      
} 


 timespec tS;           // For calculation of time
  
 tS.tv_sec = 0;

 tS.tv_nsec = 0;
 
 clock_settime(CLOCK_PROCESS_CPUTIME_ID, &tS);   // Getting time in milliseconds
    
  
 cout<<"\n\nPlease enter the SCHEDULE PERIOD : ";
 cin>>schedule_period;                             // For scheduling period

 if(schedule_period<=0)
 {
        cout<<"\n Invalid input! Please try again!\n";
            cin>>schedule_period;
 }
        
 insertBN( burst,nice);                  // For insertion of burst and nice values   
     
 assign_weight(nice,weight);             // For assigning weights to nice values
    
 total_weight = totalWeight( weight);        // Calculation of total weights         
        
 time_SL( time_slice ,schedule_period,weight,total_weight);  // Calculation of timeslice
 

 for(i=0;i<5;++i)             //For Display 
  {
          cout<<"\nTIME SLICE  : "<< time_slice[i];
          cout<<"\n";
          cout<<"  BURST TIME  : "<<burst[i]<<"\n";
          cout<<"  NICE VALUE  :   "<<nice[i]<<"\n";
          cout<<"  WEIGHTS     :" <<weight[i]<<"\n";
  }




  for(i=0;i<5;++i)          // Round Robin for simulating the scheduling environment
  {
      burst[i] = burst[i] -1 ; 
      
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tS);     // Getting time in nanoseconds
      
      initial[i] = tS.tv_nsec ;
       
      
      
      //cout << " Time taken by pid: " << i << tS.tv_nsec << endl;

      if(burst[i]>0)
      {
      
      v_runtime[i] = v_runtime[i] + ( initial[i]* (NICE_0_LOAD/weight[i])); // Calculation of V_runtime
      bst.insert(v_runtime[i],i);              // Inserting in RB TREE

      }
   }
  cout<<"\nAfter round robin \n";
   bst.printTree();            //Printing of the tree
  cout<<" \n";

  
  while(bst.getRoot())                    // CFS scheduling         
  {
       d = bst.getNext(bst.getRoot());     //Getting the next node to be scheduled
       p = bst.searchTree(d);          
       i= p->pid;

       usleep( 1000000 );

       if(burst[i] > time_slice[i])
       {
            cout<<"\n                              PROCESS ID : "<<p->pid<<" running for "<<time_slice[i]<<" units\n";  // For running
           burst[i]=burst[i]-time_slice[i];
       }
       else
       {
            cout<<"\n                              Process ID : "<<p->pid<<" running for "<<burst[i]<<" units and TERMINATED\n"; 
            burst[i]=0;

            
       }


       cout<<"\n                                   After deleting data :"<<p->data << " and ID "<<p->pid  <<"\n";
       bst.deleteNode(d);           //Deletion from the RBTREE
       cout<<"\n";
       bst.printTree();            //Printing of tree
   
       cout<<"\n";
      
       if(burst[i] > 0)
      {   
          clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tS); // Getting time in nanoseconds

          v_runtime[i] = v_runtime[i]+ (( (tS.tv_nsec + time_slice[i] )-initial[i])  * (NICE_0_LOAD/weight[i]));  // Calculation of V_runtime
          
          cout<< "\n\n V_RUNTIME to b inserted back is "<<v_runtime[i]<<"\n\n";
          
          bst.insert(v_runtime[i],i);   //Inserting into the RB tree
 
          cout<<"\n";
       
          bst.printTree();        // For displaying the RB TREE

          cout<<"\n";
      }
  }    
}
