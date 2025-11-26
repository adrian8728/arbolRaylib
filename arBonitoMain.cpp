#include <raylib.h>
#include <math.h>
#include <iostream>
#include <cstdlib> // For rand() and srand()
#include <ctime>
#include <vector>
#include <algorithm>
#include "BST.h"

using namespace std;

struct visData
{
   int val;
   float x, y;
   float theta, mag;
   int nivel;
   int size;

   visData ()
   {
      val = 0;
      x = y = 0;
      theta = mag = 0;
      size = 1;
      nivel = 0;
   }

   visData (int d)
   {
      val = d;
      x = y = 0;
      theta = mag = 0;
      nivel = 0;
      size = 1;
   }
   int operator () ()
   {
      return val;
   }
   bool operator < (visData &v)
   {
      if (val < v.val)
         return true;
      return false;
   }
};

ostream & operator << (ostream & s, visData &v)
{
   s << "[" << v.val << ", (" << v.x << ", " << v.y << "),"
      << " (" << v.mag << "," << v.theta << ") | nv= "
      << v.nivel << ", sz= " << v.size << "]";
   return s;
}

struct arBonito: public BST< visData >
{
   int ancho, alto;
   unsigned int nNiveles;
   int nodeRadius;
   float cX, cY;

   arBonito():BST< visData >()
   {
      ancho = 800;
      alto = 600;
      nNiveles = 0;
      nodeRadius = 30;
      cX = ancho / 2;
      cY = alto / 2;
   }

   arBonito (int _ancho, int _alto, BST< visData > *T = nullptr):BST< visData >(T)
   {
      ancho = _ancho;
      alto = _alto;
      nNiveles = 0;
      nodeRadius = 30;
      cX = ancho / 2;
      cY = alto / 2;
   }

   ~arBonito ()
   {
      CloseWindow ();
   }

   void definirNivelesYtamaños(){
      raiz->size = posOrder(raiz,0);
   }

   int posOrder(nodoT<visData> * nodo ,int nivel)
   {
      nodo->nivel = nivel;

      nNiveles = max(nNiveles, nivel);
      int tamaño = 1;
      if (nodo->izq)
         tamaño += posOrder(nodo->izq, nivel +1);
      
      if (nodo->der)
         tamaño+= posOrder(nodo->der, nivel +1);

      nodo->size = tamaño;
      return tamaño;
   }



   void update()
   {

   }

   void render()
   {
      BeginDrawing ();

      ClearBackground (RAYWHITE);

      EndDrawing ();
   }

   void Loop()
   {
      InitWindow (ancho, alto, "ArBonito");
      SetTargetFPS (60);

      // Main loop
      while (!WindowShouldClose ()) // Detect window close button or ESC key
      {
         //Update
         update ();

         //Draw;
         render ();
      }
   }
};

int main (int argc, char **argv)
{
   int i, N = 16;
   long semilla = 0;
   int ancho = 1280, alto = 1024;
   BST<visData> Basura;
   arBonito V(ancho, alto, &Basura);
   
   if (argc > 1)
      N = atoi (argv[1]);
   if (argc > 2)
      semilla = atol (argv[2]);

   cout << endl
      << "Se inicializó el generador de número aleatorios con "
      << semilla << endl << endl;

   //Llenamos el árbol
   //srand48 (semilla);
   srand(time(NULL));

   for (i = 0; i < N; ++i)
   {
      visData val ((int) ( rand() % 1000));

      V.inserta (val);
      cout << "Insertamos el valor " << val << " al árbol." << endl;
   }
   cout << endl;

   V.Loop ();

   return 0;
}