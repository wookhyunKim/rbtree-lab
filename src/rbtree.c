#include "rbtree.h"

#include <stdlib.h>
void right_rotate(rbtree *,node_t *);
void left_rotate(rbtree *,node_t * );
void insert_fixup(rbtree *,node_t * );
void delete_fix(rbtree *,node_t * );
void transplant(rbtree *,node_t *,node_t * );
node_t * findSuccessor(rbtree *,node_t *);
int inorder_rbtree(const rbtree *, key_t *,  int , node_t * );
void freeAction(node_t *);

// ================================ CREATE RBTREE ==============================
rbtree *new_rbtree(void)
{ 
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // malloc  node_t *nilNode = (node_t *)malloc(sizeof(node_t));
  node_t *nilNode = (node_t *)calloc(1, sizeof(node_t));

  // rbtree 의 nil 노드를 선언한 nilNode와 연결
  p->nil = nilNode;
  // rbtree root노드를 nil노드로 설정 -> 초기 상태에서 루트노드는 nil
  p->root = p->nil;

  // nil 초기화
  p->nil->left = NULL;
  p->nil->right = NULL;
  p->nil->parent = NULL;
  p->nil->color = RBTREE_BLACK;
  p->nil->key = 0;

  return p;
}
// ================================ CREATE RBTREE ==============================





node_t *rbtree_min(const rbtree *t)
{ 

  node_t * cur ;
  cur = t->root;

  while(cur->left != t->nil){
    cur = cur->left;
  }

  return cur;
}

node_t *rbtree_max(const rbtree *t)
{ // 5
  node_t * cur ;
  cur = t->root;

  while(cur->right != t->nil){
    cur = cur->right;
  }

  return cur;
}


int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{ 
  // 받은 arr에 rbtree를 key순서대로 넣기
  // 중위순회
  if (t->root != t->nil){
    inorder_rbtree(t,arr,0,t->root);
  }

  return 0;
}

int inorder_rbtree(const rbtree *t, key_t *arr, int i, node_t * cur){
  if (cur == t->nil){
    return i ;
  }
  i = inorder_rbtree(t,arr,i,cur->left);
  arr[i++] = cur->key;
  i = inorder_rbtree(t,arr,i,cur->right);
  
  return i;
}


// ================================ END OF SEARCH ==============================
// ================================ START OF Util ==============================
void left_rotate(rbtree *t, node_t *z)
{
  node_t *y = z->right;
  z->right = y->left; // y의 왼쪽 서브트리를 z의 
  if(y->left != t->nil){
    y->left->parent = z;
  }
  y->parent = z->parent;
  if(z->parent == t->nil){ // z가 루트였다면 y가 트리루트
    t->root = y;
  }else if(z == z->parent->left){
    z->parent->left = y;
  }else{
    z->parent->right = y;
  }
  y->left = z;
  z->parent = y;
}

void right_rotate(rbtree *t, node_t *z)
{
  node_t *y = z->left;
  z->left = y->right;
  if(y->right != t->nil){
    y->right->parent = z;
  }
  y->parent = z->parent;
  if(z->parent == t->nil){
    t->root = y;
  }else if(z == z->parent->right){
    z->parent->right = y;
  }else{
    z->parent->left = y;
  }
  y->right = z;
  z->parent = y;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{ 
  // t 를 순회하면서 key값이 일치하는 노드를 반환하기
  // 만약 없다면 return NULL:
  node_t *cur;
  cur = t->root;

  while (cur != t->nil)
  {
    if (cur->key == key)
    {
      return cur;
    }
    else if (cur->key > key)
    {
      // left
      cur = cur->left;
    }
    else
    {
      cur = cur->right;
    }
  }
  

  return NULL;
}
// ================================ END OF Util ==============================
// ================================ START OF INSERT ==============================

node_t *rbtree_insert(rbtree *t, const key_t key)
{ 

  node_t *y = t->nil;  // y : 부모
  node_t *x = t->root; // x : 현재 노드

  // 새 메모리 할당하여 삽입할 노드를 만들어줌 & 초기화
  node_t *z = (node_t *)calloc(1, sizeof(node_t));

  if (z == NULL){ // 메모리 할당 실패
    return NULL;
  }
  z->color = RBTREE_RED;
  z->left = t->nil;
  z->right = t->nil;
  z->parent = NULL;
  z->key = key;

  /*
  y 에 현재 x의 값을 넣어주고 x를 left or right로
  보냄으로써 y는 x의 부모가 됨을 알 수 있음.
  */
  while (x != t->nil)
  {
    y = x;
    if (key < x->key)
    {
      x = x->left;
    }
    else
    {
      x = x->right;
    }
  }

  /*
  삽일할 노드 z의 부모를 y로 설정하고
  y의 상태에 따라 혹인 y의 값에 따라  z의 자리를 결정함.
  이는 아까 설정한 x와 z가 같다라는 것을 알 수 있음
  */
  z->parent = y;
  if (y == t->nil)
  {
    t->root = z;
  }
  else if (key < y->key)
  {
    y->left = z;
  }
  else
  {
    y->right = z;
  }

  // z를 마지막노드로 하는 설정 nil노드 추가해주기
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;

  insert_fixup(t, z);
  return z;
}


void insert_fixup(rbtree *t, node_t *z)
{
  // z : cur node
  node_t * y;
  while( z->parent -> color == RBTREE_RED){
    if (z->parent == z->parent->parent->left){
      y = z->parent->parent->right;
      if (y->color == RBTREE_RED){
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }else{
        if (z== z->parent->right){
          z = z->parent;
          left_rotate(t,z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t,(z->parent->parent));
      }
    }else{
      y = z->parent->parent->left;
      if (y->color == RBTREE_RED){
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }else{
        if (z== z->parent->left){
          z = z->parent;
          right_rotate(t,z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t,(z->parent->parent));
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}


// ================================ END OF INSERT ==============================
// ================================ START OF DELETE ==============================
void delete_node(rbtree *t,node_t * node){
  if (node == t->nil) {
    return;
  }
  // 후위순회
  delete_node(t, node->left);
  delete_node(t, node->right);
  free(node);

}

void delete_rbtree(rbtree *t)
{ 

  //  t 내부에 구성요소인 root 와 nil의 메모리도 해제가 필요
  // 가리키는 곳의 메모리가 free되면 같은 곳을 가르킬 때 오류발생
  delete_node(t,t->root);
  free(t->nil);
  free(t);
}


void transplant(rbtree *t, node_t *u,node_t *v){
  if (u->parent == t->nil){
    t->root = v;
  }else if (u == u->parent->left){
    u->parent->left = v;
  }else{
    u->parent->right = v;
  }
  v->parent = u->parent;
}



int rbtree_erase(rbtree *t, node_t *p)
{ 
  // p 는 지우려는 노드임 
  node_t * y = p;
  node_t * x;
  color_t y_color = y->color;
  if (p->left == t->nil){
    x = p->right;
    transplant(t,p,p->right);
  }else if(p->right == t->nil){
    x = p->left;
    transplant(t,p,p->left);
  }else{
    y = findSuccessor(t,p->right);
    y_color = y->color;
    x = y->right;
    if (y->parent == p){
      x->parent = y;
    }else{
      transplant(t,y,y->right);
      y->right = p->right;
      y->right->parent = y;
    }
      transplant(t,p,y);
      y->left = p->left;
      y->left->parent = y;
      y->color = p->color;
  }
  if (y_color == RBTREE_BLACK){
    delete_fix(t,x);
  }
  freeAction(p);
  return 0;
}
void freeAction(node_t *p){
  free(p);
  p=NULL;
}

void delete_fix(rbtree *t, node_t *x){
  node_t * w;
  while (x!= t->root && x->color == RBTREE_BLACK){
    if (x==x->parent->left){
      w = x->parent->right;
      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED; // =-========
        left_rotate(t,x->parent);
        w = x->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }else{
        if (w->right->color == RBTREE_BLACK){
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t,w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t,x->parent);
        x = t->root;
      }
    }else{
      w = x->parent->left;
      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED; // =-========
        right_rotate(t,x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }else{
        if (w->left->color == RBTREE_BLACK){
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t,w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t,x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
  
}


node_t * findSuccessor(rbtree *t,node_t * z){
  node_t * min_node;
  min_node = z;

  while (min_node->left != t->nil){
    min_node = min_node->left;
  }
  return min_node;
}

// ================================ END OF DELETE ==============================