#include<iostream>
#include<fstream>
#include<string>
#include<list>
#include<cmath>
#include<stdlib.h>


using namespace std;

const int k = 10;  //number of nearest neighbors in k-NN test

const int n = 100; // number of examples

const int s = 50; //length of distance field (it is square)

const int p = 25;  //value of x at split



class node;

struct Link
{ node * data;
  Link * next;
  Link();
  Link(node * dat, Link * nex);
  ~Link();
};


Link::Link()
{ data = NULL;
  next = NULL; }


Link::Link(node * dat, Link * nex = NULL)
{ data = dat;
  next = nex; }

Link::~Link()
{ delete next; } 


class node{
//this is the object used to represent each arbitrary attribute holder
   
     

  public:
   void randomize(bool random_loc, bool random_class);
   int x;  
   int y;
   char foo;  //either '+' or '-' indicates the class, '?' indicates deletion or lack of initialization
   char est_foo;
   node * kNN;
   node();
   node(bool random_loc, bool random_class);
   ~node();
};



node::~node()
{  delete[] kNN; }

node::node()
: x(0), y(0), foo('?'), est_foo('?')
{ kNN = NULL; }


void node::randomize(bool random_loc, bool random_class = false)
{  kNN = NULL;
   x = rand()%s;
   y = rand()%s;
   if (!random_class)
   {  if ( x < p  )  //true if node is in the inner square
        foo = '+';
      
      else
        foo = '-';
   }        
   else
   {  if ( rand()%2 )
        foo = '+';
      else  
        foo = '-';
   }
}


node::node(bool random_loc, bool random_class = false)
{  randomize(random_loc, random_class);
}

;



class field//the object that contains all data required for the field , both attribute holders and distances
{  public:
    node ** nodes;
    Link ** kNN;
    double *  matrix; //matrix of distances
    double distance(node * a, node * b);
    double * make_matrix();
    double accuracy();
    ~field();
    field();
    field(int noise);
    void delete_tomek_links(int runs = 1);
    void estimate_class(int i);
    Link * make_NN(int);
   

};


field::~field(){}

field::field()
{  nodes = new node*[n];
   for(int i = 0; i < n; i ++)
   {  *(nodes+i) = new node(true);
   }
   matrix = make_matrix();
   kNN = new Link*[n];
  for(int i = 0; i < n; i++)
  { *(kNN+i) = make_NN(i);
  }
  
}


field::field(int noise)
{  nodes = new node*[n];
   for(int i = 0; i < n; i ++)
   {  *(nodes+i) = new node(true);
      if(rand()%100 < noise)
        *(nodes + i) = new node(true,true);
   }
   matrix = make_matrix();
   kNN = new Link*[n];
  for(int i = 0; i < n; i++)
  { *(kNN+i) = make_NN(i);
  }
  
}


double * field::make_matrix()
//for foo '+' or '-' gives distance, else gives negative number to indicate that node has been deleted
{  double * temp = new double[n*n];
   for(int i = 0; i < n; i++)
   { for(int j = 0; j < n; j++)
        temp[i*n+j] = distance(nodes[i],nodes[j]);
     
   }
   return temp;
}


Link * field::make_NN(int i)  
//makes the nearest neighbor list for the ith node in the node array, first is always closest neighbor,last is always farthest. 
{  Link * end = NULL;
   Link * root = NULL;
   double min_dis;
   double max_dis;
   int size = 1;  //gives the number of elements currently in the node list
   for(int j = 0; j < n; j += 1)
   {  double curr_dis = matrix[i*n+j];
      if(size < k)            //used to initially fill up the list with k nodes
      {  if( curr_dis < 0 )  // indicates a deleted node
           continue;
         else if (i!=j)     //for i=j distance will always be zero
         {  if(root == NULL)
            {  root = new Link(nodes[j]);
               min_dis = curr_dis;
               max_dis = curr_dis;
            }
            else if(end == NULL && curr_dis < min_dis) //used when 
            {  
               end = root;
               root = new Link(nodes[j],end);
               size += 1;
               curr_dis = min_dis;
            }
            else if( end == NULL )
            { end = new Link(nodes[j]);
              root->next = end;
              max_dis = curr_dis;
              min_dis = distance(root->data,nodes[i]);
              size += 1;
            }
                     
            else 
            {  if(curr_dis < min_dis)
               { root = new Link(nodes[j],root);
                 min_dis = curr_dis;
               }
               else if( curr_dis < max_dis )
               {  root->next = new Link(nodes[j],root->next);
               }
               else 
               {  max_dis = curr_dis;
                  end->next = new Link(nodes[j]);
                  end = end->next;
               }
            size += 1;
            }
         }
      }
      else //this section is used for when list is full
      {  if(curr_dis < 0)
           continue;
         else if(curr_dis < min_dis && i != j)
         { root = new Link(nodes[j],root);
           min_dis = curr_dis;
         }
         else if( curr_dis < max_dis && i!= j)
         {  root->next = new Link(nodes[j],root->next);
         }
         else //if curr_dis > max_dis nothing needs to be done
           continue;
         
         max_dis = 0;
         Link * curr = root;
         Link * prev = root;
         Link * prev_max_node = root; //used to store the pointer before the maximum node
         while(curr != end)   //required to find the new max node and remove old 
         {  if ( distance(nodes[j],curr->data) >= max_dis )
            { max_dis = distance(nodes[j], curr->data);
              prev_max_node = prev;
            }
            prev = curr;
            curr = curr->next;
         }
          
         delete end;     
         end = prev_max_node->next;
         prev->next = end;
         prev_max_node->next = end -> next;
         end->next = NULL;          
       
      }
   }           
  return root;
}              
             

void field::estimate_class(int i) 
{  int p = 0;
   int n = 0;
   for( Link * it = kNN[i]; it != NULL ; it = it->next)
   {  if (it->data->foo == '+')
        p+=1;
      if (it->data->foo == '-')
        n+=1;
   }
   if( p - n > 0)
     nodes[i]->est_foo = '+';
   else
     nodes[i]->est_foo = '-'; 
}


void field::delete_tomek_links(int runs)
{  bool deleted = true;
   while (deleted)
   {  deleted = false;
      for(int i = 0; i < n; i+=1)
      {  if(kNN[i] != NULL)
         {
           if( (kNN[i]->data->foo + nodes[i]->foo) == ('+' + '-') )
           {   nodes[i]->foo = '?';
               kNN[i]->data->foo = '?';
               deleted = true;
           }
           
         }
      }
      matrix = make_matrix();
      for(int i = 0; i < n; i+= 1)
      {  kNN[i] = make_NN(i); }
   
  for(int i = 0; i < n; i++)
  { *(kNN+i) = make_NN(i);
  }
}
}
      
double field::accuracy()
{ for(int i =0; i < n; i+=1)
  {  estimate_class(i); 
  }
  double right = 0;
  double total = 0;
  for(int i = 0; i < n; i += 1)
  {  if( nodes[i]->foo == nodes[i]->est_foo && nodes[i]->foo != '?' )
     { right += 1;
       total += 1;
     }
     else if(nodes[i]->foo != '?')
       total += 1;
  }
  
  return (double)right/(double)total;
}


double field::distance(node* a, node* b)
{  if ( a->foo == '?' || b->foo =='?')
      return -1;
   double temp =     sqrt( (a->x - b->x)*(a->x - b->x) + (a->y - b->y)*(a->y - b->y) );   
   return temp;
}

   
int main()
{  const int trials = 10;
   double arr[trials][11];
   double final[11];
   for(int l = 0; l < trials; l += 1)
   {   for(int i = 0; i <= 10; i += 1)
       {  srand(time(NULL));
          field boo(i*10);
          boo.delete_tomek_links();
          arr[l][i] = boo.accuracy();
         
       }
    }
   double sum=0;
   for(int i = 0; i <= 10; i += 1)
   { for(int l = 0; l < trials; l += 1)
         sum += arr[l][i];          
       final[i] = sum/trials;
       sum = 0;
   }
   

   ofstream fout("data.txt");
   for(int i = 0; i<=10; i+= 1)
   {  fout<<final[i]<<"\n"; 
      
   }



} 
    






