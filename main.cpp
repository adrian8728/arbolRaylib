// Se agregan comentarios explicativos en español sobre la intención del programa y funciones.
#include <raylib.h>
#define _USE_MATH_DEFINES //sin esto mi entorno no reconoce la constante M_PI --Adrian
#include <math.h>
#include <iostream>
#include <cstdlib> // For rand() and srand()
#include <ctime>
#include <vector>
#include <algorithm>
#include <string.h>
#include "BST.h"

using namespace std;

// Estructura que contiene los datos visuales de cada nodo (valor y datos de posición).
struct visData
{
   int val;
   float x, y;
   float theta, mag;
   int nivel;
   int size;

   // Constructor por defecto: inicializa valores a cero.
   visData ()
   {
      val = 0;
      x = y = 0;
      theta = mag = 0;
      size = 1;
      nivel = 0;
   }

   // Constructor con valor.
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
   // Comparador por valor para mantener orden en el BST.
   bool operator < (visData &v)
   {
      if (val < v.val)
         return true;
      return false;
   }
};

// Operador de salida para depuración: imprime los datos visuales del nodo.
ostream & operator << (ostream & s, visData &v)
{
   s << "[" << v.val << ", (" << v.x << ", " << v.y << "),"
      << " (" << v.mag << "," << v.theta << ") | nv= "
      << v.nivel << ", sz= " << v.size << "]";
   return s;
}

// Clase derivada de BST para gestión y visualización del árbol (Raylib).
struct arBonito: public BST< visData >
{
   int ancho, alto;
   unsigned int nNiveles;
   int nodeRadius;
   float cX, cY;
   vector<vector<nodoT<visData> * >> vecArbol;
   int tamVecArbol;

   // Inicialización por defecto de la escena/visualización.
   arBonito():BST< visData >()
   {
      ancho = GetMonitorWidth(GetCurrentMonitor());
      alto = GetMonitorHeight(GetCurrentMonitor());
      nNiveles = 0;
      nodeRadius = 20;
      cX = ancho / 2;
      cY = alto / 2;
      tamVecArbol = 0;
   }

   arBonito (int _ancho, int _alto, BST< visData > *T = nullptr):BST< visData >(T)
   {
      ancho = _ancho;
      alto = _alto;
      nNiveles = 0;
      nodeRadius = 30;
      cX = ancho / 2;
      cY = alto / 2;
      tamVecArbol = 0;
   }

   ~arBonito ()
   {
      // Cierra la ventana de Raylib si aún está abierta.
      CloseWindow ();
   }

   bool _balanceado (nodoT<visData> *r) 
   {
      if (r == nullptr)
         return true; 
      if (fabs(r->izq->dato.size - r->der->dato.size -1) <= 1)
         return false; 
      return _balanceado(r->izq) && _balanceado(r->der); 
   }

   nodoT<visData> *buscaNodoClickeado(float xRaton, float yRaton)
   {

      
      actualizarVecArbol();
      double distMin = 100000; 
      nodoT<visData> * nodoClickeado = 0;
      double umbral = 10;

      //obtener cual es el nodo que esta mas cerca de donde se clickeo
      for (int nivel = 0; nivel < nNiveles; nivel++)
         for (int i = 0; i < vecArbol[nivel].size(); i++)
         {
            nodoT<visData> * nodo = vecArbol[nivel][i];
            double dist = hypot(xRaton - nodo->dato.x,yRaton - nodo->dato.y);
            if (dist < distMin)
            {  
               distMin = dist;
               nodoClickeado = nodo;
            }   
         }
      
      if (distMin <= umbral )
         return nodoClickeado;
      else 
         return nullptr;
            
   }

   // Recorre el árbol en orden e imprime los valores (uso de depuración).
   int inOrder(nodoT<visData> *r, int nivel)
   {
      if (r == nullptr) return 0; 
      cout << r->dato.val << endl; 

      inOrder(r->izq, nivel+1); 
      inOrder(r->der, nivel+1); 
   }

    // Inicializa el número de niveles y tamaños de subárbol para cada nodo.
    void defNivelesYtamaños()
    {
      raiz->dato.size = _defNivelesYtamaños(raiz,0);

      nNiveles = nivelMaximo(raiz);
    }

    unsigned int nivelMaximo(nodoT<visData> * nodo)
    {
         if (nodo == nullptr)
            return 0;
         else if (!nodo->izq && !nodo->der)
            return nodo->dato.nivel;
         else
            return std::max(nivelMaximo(nodo->izq), nivelMaximo(nodo->izq) );
    }

    

   // Función recursiva que asigna nivel y calcula el tamaño (número de nodos) de cada subárbol.
   int _defNivelesYtamaños(nodoT<visData> * nodo ,int nivel)
   {
      nodo->dato.nivel = nivel;

      int tamaño = 1;
      if (nodo->izq)
         tamaño += _defNivelesYtamaños(nodo->izq, nivel +1);
      
      if (nodo->der)
         tamaño+= _defNivelesYtamaños(nodo->der, nivel +1);

      nodo->dato.size = tamaño;
      return tamaño;
   }

   // Reconstruye 'vecArbol' organizando punteros a nodos por nivel (para dibujado).
   void actualizarVecArbol()
   {
      //actualizamos el numero de niveles y el nivel de cada nodo
       //defNivelesYtamaños();

      //le asignamos una matriz limpia dodne el numero de filas corresponde al numero de niveles
      vector<vector<nodoT<visData> * >> arbolLimpio(nNiveles);
      vecArbol =  arbolLimpio;
      _actualizarVecArbol(raiz);
   }

   // Función recursiva que anida cada nodo en su fila correspondiente según nivel.
   void _actualizarVecArbol(nodoT<visData> * nodo)
   {
      vecArbol[nodo->dato.nivel].push_back(nodo);
      
      if (nodo->der)
         _actualizarVecArbol(nodo->der);
      
      if (nodo->izq)
         _actualizarVecArbol(nodo->izq);
   }
   // Calcula el radio (distancia al centro) para un nivel dado
   // Cada nivel se aleja menos que el anterior para que el árbol quepa mejo
   float getRadioPorNivel(int nivel)
   {
      if (nivel == 0) return 0; // La raíz está en el centro

      //float radioBase = 120.0f; // Distancia inicial del primer anillo
      float radioBase = 35.0f; // lo cambie por que para arboles con mas de 6 nivele ya no daban en la pantalla 
      //estaria bueno definir una funcion que en base al numero de niveles y el tamaño de la pantall nos de un radio base ideal
      
      float radioAcumulado = 0;
      
      // Sumatoria con decaimiento: dist + dist*0.8 + dist*0.64...
      for(int i = 1; i <= nivel; i++) {
         // Factor de decaimiento: cada nivel es el 85% del ancho del anterior
         radioAcumulado += radioBase * pow(1.02f, i - 1);
      }
      return radioAcumulado;
   }

   // Función principal para iniciar el cálculo de posiciones
   void calcularPosiciones()
   {
      if (raiz == nullptr) return;

      // Actualizamos los tamaños de los subárboles
      defNivelesYtamaños(); 

      // La raíz abarca todo el círculo (0 a 2*PI)
      _calcularPosiciones(raiz, 0.0f, 2.0f * M_PI);
   }

   void _calcularPosiciones(nodoT<visData> *nodo, float anguloInicio, float anguloFin)
   {
      if (nodo == nullptr) return;

      // 1. Determinar posición del nodo actual (Centro del sector)
      float anguloCentro = (anguloInicio + anguloFin)/ 2.0f;
      float radio = getRadioPorNivel(nodo->dato.nivel);

      // Guardamos coordenadas polares
      nodo->dato.theta = anguloCentro;
      nodo->dato.mag = radio;

      // Convertimos a Cartesianas
      nodo->dato.x = cX + radio * cos(anguloCentro);
      nodo->dato.y = cY + radio * sin(anguloCentro);

      // --- PREPARACIÓN PARA HIJOS ---
      nodoT<visData> *izq = nodo->izq;
      nodoT<visData> *der = nodo->der;

      if (izq == nullptr && der == nullptr) return;
      
      double sizeIzq = (izq != nullptr) ? (double)izq->dato.size : 0;
      double sizeDer = (der != nullptr) ? (double)der->dato.size : 0;
      double totalHijos = sizeIzq + sizeDer;

      float anchoSector = anguloFin - anguloInicio;
      float anchoIzq = (totalHijos > 0) ? anchoSector * (float)(sizeIzq / totalHijos) : 0;

            /******************************
            ********* NO TOCAR ************
            **** CREO QUE YA FUNCIONA *****
            ******************************/
      if (nodo->dato.nivel == 0)
      {
         // Hemisferio Izquierdo para el hijo 'izq' (Centrado en 0)
         if (izq != nullptr) {
            _calcularPosiciones(izq, -anchoIzq / 2.0f, anchoIzq/2.0f);

         }

         // Hemisferio Derecho para el hijo 'der' (Centrado en 180)
         if (der != nullptr) {
            _calcularPosiciones(der, anchoIzq / 2.0f, 2 * M_PI - (anchoIzq/2.0f));
         }
         return; // Terminamos aquí para la raíz, no ejecutamos la lógica estándar de abajo.
      }

      if (izq != nullptr) {
         _calcularPosiciones(izq, anguloInicio, anguloInicio + anchoIzq);
      }

      if (der != nullptr) {
         _calcularPosiciones(der, anguloInicio + anchoIzq, anguloFin);
      }
   }
   
   // update: lugar para la lógica de actualización por frame (animaciones, cálculo de posiciones).
   void update()
   {
         Vector2 posicionMouse = GetMousePosition();

         

         if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
         {
            nodoT<visData>* nodoClickeado = buscaNodoClickeado((float) posicionMouse.x,(float)posicionMouse.y);
            if (nodoClickeado)
            {
               DrawText(TextFormat("ultimo nodo clickeado: %i",nodoClickeado->dato.val),  cX, cY,15, DARKBLUE);
               //comente esto pq a veces funciona a veces se petatea, hay que debuggearlo
               
               extraeNodo(nodoClickeado);
               inserta(nodoClickeado);
            }
         }
         calcularPosiciones();
         actualizarVecArbol(); 

   }

   // render: lugar para dibujar el árbol en pantalla con Raylib.
   void render()
   {
      BeginDrawing ();
      ancho = GetMonitorWidth(GetCurrentMonitor());
      alto = GetMonitorHeight(GetCurrentMonitor());
      cX = ancho/2;
      cY = alto/2;

      ClearBackground (RAYWHITE);

      for (int i = 1; i < nNiveles; i++)
      {
         DrawCircleLines(cX, cY, getRadioPorNivel(i),  DARKBLUE);
      }

      
      dibujarArbol(raiz);

      EndDrawing ();
   }

   void dibujarArbol(nodoT<visData> *nodo)
   {
      
      DrawCircleLines(nodo->dato.x, nodo->dato.y, nodeRadius,  GREEN);
      DrawText(TextFormat("%i",nodo->dato.val),  nodo->dato.x, nodo->dato.y,12, DARKBLUE);

      if (nodo->padre)
         DrawLine(nodo->padre->dato.x, nodo->padre->dato.y, nodo->dato.x, nodo->dato.y, RED);

      if (nodo->izq)
         dibujarArbol(nodo->izq);
      
      if (nodo->der)
         dibujarArbol(nodo->der);
   }

   // Bucle principal de la aplicación: inicializa ventana y ejecuta update/render.
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
   int i, N = 40;
   long semilla = 0;
   int ancho = 1920, alto = 1080;
   BST<visData> Basura;
   //arBonito V(ancho, alto, &Basura);
   arBonito V;

   cout << "alto: " << GetScreenHeight() << endl;
   cout << "ancho: " << V.ancho << endl;
   
   
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

   V.defNivelesYtamaños();

   V.Loop ();

   return 0;
}