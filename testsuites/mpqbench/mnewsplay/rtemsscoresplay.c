
#include <rtems/system.h>
#include "rtemsscoresplay.h"

/*
 *  The implementation used here is based on the implementation
 *  which was used in the tests of splay trees reported in:
 *
 *    An Empirical Comparison of Priority-Queue and Event-Set Implementations,
 *	by Douglas W. Jones, Comm. ACM 29, 4 (Apr. 1986) 300-311.
 *
 *  The changes made include the addition of the enqprior
 *  operation and the addition of up-links to allow for the splay
 *  operation.  The basic splay tree algorithms were originally
 *  presented in:
 *
 *	Self Adjusting Binary Trees,
 *		by D. D. Sleator and R. E. Tarjan,
 *			Proc. ACM SIGACT Symposium on Theory
 *			of Computing (Boston, Apr 1983) 235-245.
 *
 *  The enq and enqprior routines use variations on the
 *  top-down splay operation, while the splay routine is bottom-up.
 *  All are coded for speed.
 *
 *  Written by:
 *    Douglas W. Jones
 *
 *  Translated to C by:
 *    David Brower, daveb@rtech.uucp
 *
 * Thu Oct  6 12:11:33 PDT 1988 (daveb) Fixed _Splay_Dequeue, which was broken
 *	handling one-node trees.  I botched the pascal translation of
 *	a VAR parameter.
 */

bool _Splay_Is_empty( Splay_Control *the_tree )
{
  return the_tree == NULL || the_tree->root == NULL;
}

/*----------------
 *
 *  _Splay_Insert() -- insert item in a tree.
 *
 *  put the_node in the_tree after all other nodes with the same key; when this is
 *  done, the_node will be the root of the splay tree representing the_tree, all nodes
 *  in the_tree with keys less than or equal to that of the_node will be in the
 *  left subtree, all with greater keys will be in the right subtree;
 *  the tree is split into these subtrees from the top down, with rotations
 *  performed along the way to shorten the left branch of the right subtree
 *  and the right branch of the left subtree
 */
Splay_Node * _Splay_Insert( Splay_Control *the_tree, Splay_Node *the_node )
{
  Splay_Node * left;  /* the rightmost node in the left tree */
  Splay_Node * right;  /* the leftmost node in the right tree */
  Splay_Node * next;  /* the root of the unsplit part */
  Splay_Node * temp;

  the_tree->enqs++;
  the_node->parent = NULL;
  next = the_tree->root;
  the_tree->root = the_node;
  
  if( next == NULL )  /* trivial enq */
  {
    the_node->child[TREE_LEFT] = NULL;
    the_node->child[TREE_RIGHT] = NULL;
    return the_node;
  }
  
  /* difficult enq */
  left = the_node;
  right = the_node;

  /* the_node's left and right children will hold the right and left
     splayed trees resulting from splitting on the_node->key;
     note that the children will be reversed! */

  the_tree->enqcmps++;

  /* figure out which side to start on */
  if ( the_tree->compare_function(next, the_node) > 0 )
    goto two;

one:  /* assert next->key <= key */

  do  /* walk to the right in the left tree */
  {
    temp = next->child[TREE_RIGHT];
    if( temp == NULL )
    {
      left->child[TREE_RIGHT] = next;
      next->parent = left;
      right->child[TREE_LEFT] = NULL;
      goto done;  /* job done, entire tree split */
    }

    the_tree->enqcmps++;
    if( the_tree->compare_function(temp, the_node) > 0 )
    {
      left->child[TREE_RIGHT] = next;
      next->parent = left;
      left = next;
      next = temp;
      goto two;  /* change sides */
    }

    next->child[TREE_RIGHT] = temp->child[TREE_LEFT];
    if( temp->child[TREE_LEFT] != NULL )
      temp->child[TREE_LEFT]->parent = next;
    left->child[TREE_RIGHT] = temp;
    temp->parent = left;
    temp->child[TREE_LEFT] = next;
    next->parent = temp;
    left = temp;
    next = temp->child[TREE_RIGHT];
    if( next == NULL )
    {
      right->child[TREE_LEFT] = NULL;
      goto done;  /* job done, entire tree split */
    }

    the_tree->enqcmps++;

  } while( the_tree->compare_function(next, the_node) <= 0 );  /* change sides */

two:  /* assert next->key > key */

  do  /* walk to the left in the right tree */
  {
    temp = next->child[TREE_LEFT];
    if( temp == NULL )
    {
      right->child[TREE_LEFT] = next;
      next->parent = right;
      left->child[TREE_RIGHT] = NULL;
      goto done;  /* job done, entire tree split */
    }

    the_tree->enqcmps++;
    if( the_tree->compare_function(temp, the_node) <= 0 )
    {
      right->child[TREE_LEFT] = next;
      next->parent = right;
      right = next;
      next = temp;
      goto one;  /* change sides */
    }
    next->child[TREE_LEFT] = temp->child[TREE_RIGHT];
    if( temp->child[TREE_RIGHT] != NULL )
      temp->child[TREE_RIGHT]->parent = next;
    right->child[TREE_LEFT] = temp;
    temp->parent = right;
    temp->child[TREE_RIGHT] = next;
    next->parent = temp;
    right = temp;
    next = temp->child[TREE_LEFT];
    if( next == NULL )
    {
      left->child[TREE_RIGHT] = NULL;
      goto done;  /* job done, entire tree split */
    }

    the_tree->enqcmps++;

  } while( the_tree->compare_function(next, the_node) > 0 );  /* change sides */

  goto one;

done:  /* split is done, branches of the_node need reversal */

  temp = the_node->child[TREE_LEFT];
  the_node->child[TREE_LEFT] = the_node->child[TREE_RIGHT];
  the_node->child[TREE_RIGHT] = temp;

  return( the_node );

} /* _Splay_Insert */


/*----------------
 *
 *  _Splay_Dequeue() -- return and remove head node from a subtree.
 *
 *  remove and return the head node from the node set; this deletes
 *  (and returns) the leftmost node from the_tree, replacing it with its right
 *  subtree (if there is one); on the way to the leftmost node, rotations
 *  are performed to shorten the left branch of the tree
 */
Splay_Node *_Splay_Dequeue( Splay_Node **np )
{
  Splay_Node * deq;    /* one to return */
  Splay_Node * next;   /* the next thing to deal with */
  Splay_Node * left;  /* the left child of next */
  Splay_Node * farleft;    /* the left child of left */
  Splay_Node * farfarleft;  /* the left child of farleft */

  if( np == NULL || *np == NULL )
  {
    deq = NULL;
  }
  else
  {
    next = *np;
    left = next->child[TREE_LEFT];
    if( left == NULL )
    {
      deq = next;
      *np = next->child[TREE_RIGHT];

      if( *np != NULL )
  (*np)->parent = NULL;

    }
    else for(;;)  /* left is not null */
    {
      /* next is not it, left is not NULL, might be it */
      farleft = left->child[TREE_LEFT];
      if( farleft == NULL )
      {
  deq = left;
  next->child[TREE_LEFT] = left->child[TREE_RIGHT];
  if( left->child[TREE_RIGHT] != NULL )
    left->child[TREE_RIGHT]->parent = next;
  break;
      }

      /* next, left are not it, farleft is not NULL, might be it */
      farfarleft = farleft->child[TREE_LEFT];
      if( farfarleft == NULL )
      {
  deq = farleft;
  left->child[TREE_LEFT] = farleft->child[TREE_RIGHT];
  if( farleft->child[TREE_RIGHT] != NULL )
    farleft->child[TREE_RIGHT]->parent = left;
  break;
      }

      /* next, left, farleft are not it, rotate */
      next->child[TREE_LEFT] = farleft;
      farleft->parent = next;
      left->child[TREE_LEFT] = farleft->child[TREE_RIGHT];
      if( farleft->child[TREE_RIGHT] != NULL )
  farleft->child[TREE_RIGHT]->parent = left;
      farleft->child[TREE_RIGHT] = left;
      left->parent = farleft;
      next = farleft;
      left = farfarleft;
    }
  }

  return( deq );

} /* _Splay_Dequeue */


/*----------------
 *
 *  _Splay_Insert_before() -- insert into tree before other equal keys.
 *
 *  put the_node in the_tree before all other nodes with the same key; after this is
 *  done, the_node will be the root of the splay tree representing the_tree, all nodes in
 *  the_tree with keys less than that of the_node will be in the left subtree, all with
 *  greater or equal keys will be in the right subtree; the tree is split
 *  into these subtrees from the top down, with rotations performed along
 *  the way to shorten the left branch of the right subtree and the right
 *  branch of the left subtree; the logic of _Splay_Insert_before is exactly the
 *  same as that of _Splay_Insert except for a substitution of comparison
 *  operators
 */
Splay_Node *_Splay_Insert_before(Splay_Control *the_tree, Splay_Node *the_node)
{
  Splay_Node * left;  /* the rightmost node in the left tree */
  Splay_Node * right;  /* the leftmost node in the right tree */
  Splay_Node * next;  /* the root of unsplit part of tree */
  Splay_Node * temp;

  the_node->parent = NULL;
  next = the_tree->root;
  the_tree->root = the_node;
  if( next == NULL )  /* trivial enq */
  {
    the_node->child[TREE_LEFT] = NULL;
    the_node->child[TREE_RIGHT] = NULL;
    return the_node;
  }
  /* difficult enq */
  left = the_node;
  right = the_node;

  /* the_node's left and right children will hold the right and left
     splayed trees resulting from splitting on the_node->key;
     note that the children will be reversed! */

  if( the_tree->compare_function(next, the_node) >= 0 )
    goto two;

one:  /* assert next->key < key */

  do  /* walk to the right in the left tree */
  {
    temp = next->child[TREE_RIGHT];
    if( temp == NULL )
    {
      left->child[TREE_RIGHT] = next;
      next->parent = left;
      right->child[TREE_LEFT] = NULL;
      goto done;  /* job done, entire tree split */
    }
    if( the_tree->compare_function(temp, the_node) >= 0 )
    {
      left->child[TREE_RIGHT] = next;
      next->parent = left;
      left = next;
      next = temp;
      goto two;  /* change sides */
    }
    next->child[TREE_RIGHT] = temp->child[TREE_LEFT];
    if( temp->child[TREE_LEFT] != NULL )
      temp->child[TREE_LEFT]->parent = next;
    left->child[TREE_RIGHT] = temp;
    temp->parent = left;
    temp->child[TREE_LEFT] = next;
    next->parent = temp;
    left = temp;
    next = temp->child[TREE_RIGHT];
    if( next == NULL )
    {
      right->child[TREE_LEFT] = NULL;
      goto done;  /* job done, entire tree split */
    }

  } while( the_tree->compare_function( next, the_node ) < 0 );  /* change sides */

two:  /* assert next->key >= key */

  do   /* walk to the left in the right tree */
  {
    temp = next->child[TREE_LEFT];
    if( temp == NULL )
    {
      right->child[TREE_LEFT] = next;
      next->parent = right;
      left->child[TREE_RIGHT] = NULL;
      goto done;  /* job done, entire tree split */
    }
    if( the_tree->compare_function(temp, the_node) < 0 )
    {
      right->child[TREE_LEFT] = next;
      next->parent = right;
      right = next;
      next = temp;
      goto one;  /* change sides */
    }
    next->child[TREE_LEFT] = temp->child[TREE_RIGHT];
    if( temp->child[TREE_RIGHT] != NULL )
      temp->child[TREE_RIGHT]->parent = next;
    right->child[TREE_LEFT] = temp;
    temp->parent = right;
    temp->child[TREE_RIGHT] = next;
    next->parent = temp;
    right = temp;
    next = temp->child[TREE_LEFT];
    if( next == NULL )
    {
      left->child[TREE_RIGHT] = NULL;
      goto done;  /* job done, entire tree split */
    }

  } while( the_tree->compare_function(next, the_node) >= 0 );  /* change sides */

  goto one;

done:  /* split is done, branches of the_node need reversal */

  temp = the_node->child[TREE_LEFT];
  the_node->child[TREE_LEFT] = the_node->child[TREE_RIGHT];
  the_node->child[TREE_RIGHT] = temp;

  return( the_node );

} /* _Splay_Insert_before */

/*----------------
 *
 *  _Splay_Splay() -- reorganize the tree.
 *
 *  the tree is reorganized so that the_node is the root of the
 *  splay tree representing the_tree; results are unpredictable if the_node is not
 *  in the_tree to start with; the_tree is split from the_node up to the old root, with all
 *  nodes to the left of the_node ending up in the left subtree, and all nodes
 *  to the right of the_node ending up in the right subtree; the left branch of
 *  the right subtree and the right branch of the left subtree are
 *  shortened in the process
 *
 *  this code assumes that the_node is not NULL and is in the_tree;
 *  it can sometimes detect the_node not in the_tree and complain
 */
void _Splay_Splay( Splay_Control *the_tree, Splay_Node *the_node )
{
  Splay_Node * up;  /* points to the node being dealt with */
  Splay_Node * prev;  /* a descendent of up, already dealt with */
  Splay_Node * upup;  /* the parent of up */
  Splay_Node * upupup;  /* the grandparent of up */
  Splay_Node * left;  /* the top of left subtree being built */
  Splay_Node * right;  /* the top of right subtree being built */

  left = the_node->child[TREE_LEFT];
  right = the_node->child[TREE_RIGHT];
  prev = the_node;
  up = prev->parent;

  the_tree->splays++;

  while( up != NULL )
  {
    the_tree->splayloops++;

    /* walk up the tree towards the root, splaying all to the left of
       the_node into the left subtree, all to right into the right subtree */

    upup = up->parent;
    if( up->child[TREE_LEFT] == prev )  /* up is to the right of the_node */
    {
      if( upup != NULL && upup->child[TREE_LEFT] == up )  /* rotate */
      {
  upupup = upup->parent;
  upup->child[TREE_LEFT] = up->child[TREE_RIGHT];
  if( upup->child[TREE_LEFT] != NULL )
    upup->child[TREE_LEFT]->parent = upup;
  up->child[TREE_RIGHT] = upup;
  upup->parent = up;
  if( upupup == NULL )
    the_tree->root = up;
  else if( upupup->child[TREE_LEFT] == upup )
    upupup->child[TREE_LEFT] = up;
  else
    upupup->child[TREE_RIGHT] = up;
  up->parent = upupup;
  upup = upupup;
      }
      up->child[TREE_LEFT] = right;
      if( right != NULL )
  right->parent = up;
      right = up;

    }
    else  /* up is to the left of the_node */
    {
      if( upup != NULL && upup->child[TREE_RIGHT] == up )  /* rotate */
      {
  upupup = upup->parent;
  upup->child[TREE_RIGHT] = up->child[TREE_LEFT];
  if( upup->child[TREE_RIGHT] != NULL )
    upup->child[TREE_RIGHT]->parent = upup;
  up->child[TREE_LEFT] = upup;
  upup->parent = up;
  if( upupup == NULL )
    the_tree->root = up;
  else if( upupup->child[TREE_RIGHT] == upup )
    upupup->child[TREE_RIGHT] = up;
  else
    upupup->child[TREE_LEFT] = up;
  up->parent = upupup;
  upup = upupup;
      }
      up->child[TREE_RIGHT] = left;
      if( left != NULL )
  left->parent = up;
      left = up;
    }
    prev = up;
    up = upup;
  }

# ifdef DEBUG
  if( the_tree->root != prev )
  {
    /*  fprintf(stderr, " *** bug in splay: the_node not in the_tree *** " ); */
    abort();
  }
# endif

  the_node->child[TREE_LEFT] = left;
  the_node->child[TREE_RIGHT] = right;
  if( left != NULL )
    left->parent = the_node;
  if( right != NULL )
    right->parent = the_node;
  the_tree->root = the_node;
  the_node->parent = NULL;

} /* splay */
/* /sptree.c */

/**
 *  _Splay_Find
 *
 *  Searches for a particular key in the tree returning the first one found.
 *  If a node is found splay it and return it. Returns NULL if none is found.
 */
Splay_Node *_Splay_Find(Splay_Control *tree, Splay_Node *search_node)
{
  Splay_Node *iter = tree->root;
  while ( iter ) {
    if(tree->compare_function(iter, search_node) > 0) {
      iter = iter->child[TREE_LEFT];
    }
    else if(tree->compare_function(iter, search_node) < 0) {
      iter = iter->child[TREE_RIGHT];
    }
    else {
      _Splay_Splay(tree, iter);
      break;
    }
  }
  return iter;
}

/**
 * spget_successor
 *
 * Find the in-order successor of the_node. Assumes it exists
 */
static Splay_Node* spget_successor( Splay_Node *the_node ) {
  the_node = the_node->child[TREE_RIGHT];
  while ( the_node->child[TREE_LEFT] ) {
    the_node = the_node->child[TREE_LEFT];
  }
  return the_node;
}

/**
 *  _Splay_Extract
 *
 *  Searches for and removes a particular key in the tree.
 *  If a node is pruned then return it. Return NULL if key is not found.
 */
Splay_Node* _Splay_Extract(Splay_Control *tree, Splay_Node *search_node)
{
  Splay_Node *node = _Splay_Find(tree, search_node);
  Splay_Node *new_root = NULL;
  Splay_Node *left;
  Splay_Node *right;

  if ( node ) {
    /* because of splaying in _Splay_Find() node is the root */
    if ( node->child[TREE_LEFT] == NULL ) {
      /* replace with right child if it exists */
      right = node->child[TREE_RIGHT];
      if ( right ) {
        right->parent = NULL;
      }
      tree->root = right;
    } else if ( node->child[TREE_RIGHT] == NULL ) {
      /* replace with left child, which exists */
      left = node->child[TREE_LEFT];
      left->parent = NULL;
      tree->root = left;
    } else {
      /* replace node with its successor */
      Splay_Node *suc = spget_successor(node);
      right = node->child[TREE_RIGHT];
      left = node->child[TREE_LEFT];
      if ( suc != node->child[TREE_RIGHT] ) {
        new_root = suc->parent;
        if ( suc->child[TREE_RIGHT] ) {
          suc->child[TREE_RIGHT]->parent = new_root;
          new_root->child[TREE_LEFT] = suc->child[TREE_RIGHT];
        } else {
          new_root->child[TREE_LEFT] = NULL;
        }
        suc->parent = node->parent;
        node->child[TREE_RIGHT]->parent = suc;
        node->child[TREE_LEFT]->parent = suc;
        suc->child[TREE_RIGHT] = node->child[TREE_RIGHT];
        suc->child[TREE_LEFT] = node->child[TREE_LEFT];
      } else { /* successor is the right child */
        new_root = suc;
        suc->parent = node->parent;
        node->child[TREE_LEFT]->parent = suc;
        suc->child[TREE_LEFT] = node->child[TREE_LEFT];
      }
      tree->root = suc;
      _Splay_Splay(tree, new_root);
    }
  }
  return node;
}

