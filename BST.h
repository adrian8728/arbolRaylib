#ifndef __BST__
#define __BST__

/* Binary Search Tree - Árbol de Búsqueda Binario */

#include "nodoT.h"
#include <iostream>

template < typename X > struct BST
{
   nodoT < X > *raiz;
   BST < X > *trash;         //trash -> basura en ingles.

   // Constructor por defecto: inicializa un árbol vacío.
   BST()
   {
      raiz = nullptr;
      trash = nullptr;
   }

   // Constructor que puede recibir un "trash" (lista de nodos reutilizables).
   BST (BST < X > *T)
   {
      raiz = nullptr;
      trash = T;
   }

   // Constructor copia: delega en el operador= para realizar la copia.
   BST (const BST < X > &L)
   {
      raiz = nullptr;
      trash = nullptr;
      *this = L;
   }

   ~BST ()
   {
      // Destructor: borra todo el árbol para liberar memoria.
      borraArbol ();
   }

   // Copia el contenido de 'l' en este árbol (borra el actual si existe).
   void copia (const BST < X > &l)
   {
      if (raiz != nullptr) 
         borraArbol();

      _copia(l.raiz);
   }

   // Función recursiva auxiliar para copiar nodos (recorrido preorden).
   void _copia(nodoT<X> *r)
   {
      if (r == nullptr)
         return;
      inserta(r->dato);
      _copia(r->izq);
      _copia(r->der);
   }

   BST < X > &operator = (const BST < X > &l)
   {
      copia(l);
      return *this;
   }

   // Borra todo el árbol usando la rutina recursiva _borraArbol.
   void borraArbol ()
   {
      _borraArbol(raiz);
      raiz = nullptr;
   }

   // Recorrido post-order para eliminar nodos de manera segura.
   void _borraArbol(nodoT<X> *r)
   {
      if (r == nullptr)
         return;
      _borraArbol(r->izq);
      _borraArbol(r->der);
      borraNodo(r);
   }

   // Elimina un nodo: lo extrae del árbol y lo borra o lo añade a 'trash' para reutilización.
   void borraNodo (nodoT < X > *n)
   {
      if (n == nullptr)
         return;
      n = extraeNodo(n); //Desconectamos al nodo n del árbol.
      if (trash != 0)
      {  
         n->der = trash->raiz;
         trash->raiz = n;
      }
      else
         delete n;
   }

   // Crea un nuevo nodo: reutiliza uno de 'trash' si está disponible, sino usa new.
   nodoT < X > *creaNodo (const X & d)
   {
      nodoT < X > *n;

      if (trash != nullptr && trash->raiz != nullptr)
      {
         n = trash->raiz;
         trash->raiz = trash->raiz->der;
         n->izq = n->der = n->padre = nullptr;
         n->dato = d;
      }
      else
         n = new nodoT < X > (d);
      return n;
   }

   // Inserta un nodo ya creado en la posición adecuada (sin romper punteros).
   void inserta(nodoT<X> *n)
   {
      nodoT<X> *x, *y;

      if (n == nullptr)
         return;
      if (raiz == nullptr)
      {
         raiz = n;
         return;
      }
      x = raiz;
      y = nullptr;
      while (x != nullptr)
      {
         y = x;
         if (n->dato < x->dato)
            x = x->izq;
         else
            x = x->der;
      }
      if (n->dato < y->dato)
            y->izq = n;
         else
            y->der = n;
      n->padre = y;
   }

   // Inserta un valor creando primero un nodo con creaNodo.
   void inserta(X &d)
   {
      nodoT<X> *n;

      n = creaNodo(d);
      if (n != nullptr)
         inserta(n);
   }

   // Búsqueda iterativa de un valor: devuelve el puntero al nodo o nullptr.
   nodoT<X> *busca(X &val)
   {
      nodoT<X> *apu = raiz;

      while (apu != nullptr)
      {
         if (apu->dato == val)
            return apu;
         if (val < apu->dato)
            apu = apu->izq;
         else
            apu = apu->der;
      }
      return nullptr;
   }

   // Recorridos clásicos: inorder, preorder, postorder (imprimen en std::cout).
   void inorder()
   {
      _inorder(raiz);
   }

   void _inorder(nodoT<X> *r)
   {
      if (r == nullptr)
         return;
      _inorder(r->izq);
      std::cout << r->dato << " ";
      _inorder(r->der);
   }

   void inorderInverso()
   {
      _inorderInverso(raiz);
   }

   void _inorderInverso(nodoT<X> *r)
   {
      if (r == nullptr)
         return;
      _inorderInverso(r->der);
      std::cout << r->dato << " ";
      _inorderInverso(r->izq);
   }

   void preorder()
   {
      _preorder(raiz);
   }

   void _preorder(nodoT<X> *r)
   {
      if (r == nullptr)
         return;
      std::cout << r->dato << " ";
      _preorder(r->izq);
      _preorder(r->der);
   }

   void posorder()
   {
      _posorder(raiz);
   }

   void _posorder(nodoT<X> *r)
   {
      if (r == nullptr)
         return;
      _posorder(r->izq);      
      _posorder(r->der);
      std::cout << r->dato << " ";
   }

   // Métodos para obtener mínimo/máximo (iterativos y recursivos).
   nodoT<X> *min()
   {
      nodoT<X> *apu = raiz;

      if (raiz == nullptr)
         return nullptr;

      while(apu->izq)
         apu = apu->izq;
      return apu;
   }

   nodoT<X> *max()
   {
      nodoT<X> *apu = raiz;

      if (raiz == nullptr)
         return nullptr;

      while(apu->der)
         apu = apu->der;
      return apu;
   }

   nodoT<X> *min(nodoT<X> *apu)
   {
      if (apu == nullptr)
         return nullptr;

      while(apu->izq)
         apu = apu->izq;
      return apu;
   }

   nodoT<X> *max(nodoT<X> *apu)
   {
      if (apu == nullptr)
         return nullptr;

      while(apu->der)
         apu = apu->der;
      return apu;
      
   }

   nodoT<X> *minR()
   {
      if (!raiz) return nullptr;
      return _minR(raiz);
   }

   nodoT<X> *_minR(nodoT<X> *r)
   {
      if (r->izq == nullptr)
         return r;
       else
         return _minR(r->izq);
   }

   nodoT<X> *maxR()
   {
      if (!raiz) return nullptr;
      return _maxR(raiz);
   }

   nodoT<X> *_maxR(nodoT<X> *r)
   {
      if (r->der == nullptr)
         return r;
       else
         return _maxR(r->der);
   }

   // Sucesor y predecesor en orden in-order (útil para recorridos iterativos).
   nodoT<X> *sucesor(nodoT<X> *apu)
   {
      nodoT<X> *x, *y;

      if (apu == nullptr)
         return nullptr;

      if (apu->der != nullptr)
         return min(apu->der);

      x = apu;
      y = apu->padre;
      while (y != nullptr && y->der == x)
      {
         x=y;
         y=y->padre;
      }
      return y;
   }

   nodoT<X> *predecesor(nodoT<X> *apu)
   {
      nodoT<X> *x, *y;

      if (apu == nullptr)
         return nullptr;

      if (apu->izq != nullptr)
         return max(apu->izq);

      x = apu;
      y = apu->padre;
      while (y != nullptr && y->izq == x)
      {
         x=y;
         y=y->padre;
      }
      return y;
   }

   // Recorridos iterativos usando sucesor/predecesor.
   void recorridoIterativo()
   {
      nodoT<X> *apu;

      apu = min();
      while (apu != nullptr)
      {
         std::cout << apu->dato << " ";
         apu = sucesor(apu);
      }
   }

   void recorridoIterativoInverso()
   {
      nodoT<X> *apu;

      apu = max();
      while (apu != nullptr)
      {
         std::cout << apu->dato << " ";
         apu = predecesor(apu);
      }
   }

   // Extrae un nodo 'z' del árbol manteniendo la estructura del BST y devuelve el nodo desconectado.
   nodoT<X> *extraeNodo(nodoT<X> *z)
   {
      nodoT<X> *x, *y;

      if (z->izq == nullptr || z->der == nullptr)
         y = z;
      else
         y = sucesor(z);
      if (y->izq != nullptr)
         x = y->izq;
      else
         x = y->der;
      if (x != nullptr)
         x->padre = y->padre;
      if (y->padre == nullptr)
         raiz = x;
      else
      {
         if (y == y->padre->izq)
            y->padre->izq = x;
         else
            y->padre->der = x;
      }
      if (y != z)
         z->dato = y->dato;
      y->izq = y->der = y->padre = nullptr;
      return y;
   }   
};

template < typename X >
void imprime(nodoT<X> *r, std::ostream &s)
{
   if (!r)
      return;
   imprime(r->izq, s);
   s << r->dato << " ";
   imprime(r->der, s);
}

template < typename X >
   std::ostream & operator << (std::ostream & s, BST < X > &t)
{
   s << "raiz->";
   imprime(t.raiz, s);
   s << "[]";
   return s;
}

#endif
