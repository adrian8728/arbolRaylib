// Se agregan comentarios explicativos en español sobre la intención del programa y funciones.
#include <raylib.h>
#include <math.h>
#include <iostream>
#include <cstdlib> // For rand() and srand()
#include <ctime>
#include <vector>
#include <algorithm>
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
      ancho = 800;
      alto = 600;
      nNiveles = 0;
      nodeRadius = 30;
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
    }

   // Función recursiva que asigna nivel y calcula el tamaño (número de nodos) de cada subárbol.
   int _defNivelesYtamaños(nodoT<visData> * nodo ,int nivel)
   {
      nodo->dato.nivel = nivel;

      nNiveles = std::max(nNiveles, (unsigned int) nivel + 1);
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
       defNivelesYtamaños();

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

      float radioBase = 120.0f; // Distancia inicial del primer anillo
      float radioAcumulado = 0;
      
      // Sumatoria con decaimiento: dist + dist*0.8 + dist*0.64...
      for(int i = 1; i <= nivel; i++) {
         // Factor de decaimiento: cada nivel es el 85% del ancho del anterior
         radioAcumulado += radioBase * pow(0.85f, i - 1);
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

      // Determinar posición del nodo actual
      // Se coloca al CENTRO de su sector asignado
      float anguloCentro = anguloInicio + (anguloFin - anguloInicio) / 2.0f;
      float radio = getRadioPorNivel(nodo->dato.nivel);

      // Guardamos coordenadas polares
      nodo->dato.theta = anguloCentro;
      nodo->dato.mag = radio;

      // Convertimos a Cartesianas (x, y) relativas al centro de pantalla (cX, cY)
      nodo->dato.x = cX + radio * cos(anguloCentro);
      nodo->dato.y = cY + radio * sin(anguloCentro);

      // 2. Calcular la distribución para los hijos (si existen)
      nodoT<visData> *izq = nodo->izq;
      nodoT<visData> *der = nodo->der;

      if (izq == nullptr && der == nullptr) return; // Es hoja, terminamos

      // Obtenemos los tamaños (size) calculados previamente en defNivelesYtamaños
      // Usamos double para evitar problemas de división entera
      double sizeIzq = (izq != nullptr) ? (double)izq->dato.size : 0;
      double sizeDer = (der != nullptr) ? (double)der->dato.size : 0;
      double totalHijos = sizeIzq + sizeDer;

      // Ancho total del sector actual en radianes
      float anchoSector = anguloFin - anguloInicio;

      // Calculamos qué fracción del ángulo le toca al hijo izquierdo
      float anchoIzq = (totalHijos > 0) ? anchoSector * (float)(sizeIzq / totalHijos) : 0;

      // Llamadas recursivas pasando los nuevos rangos angulares
      // El hijo izquierdo toma desde [Inicio] hasta [Inicio + anchoIzq]
      if (izq != nullptr) {
         _calcularPosiciones(izq, anguloInicio, anguloInicio + anchoIzq);
      }

      // El hijo derecho toma el resto: desde [Inicio + anchoIzq] hasta [Fin]
      if (der != nullptr) {
         _calcularPosiciones(der, anguloInicio + anchoIzq, anguloFin);
      }
   }
   
   // update: lugar para la lógica de actualización por frame (animaciones, cálculo de posiciones).
   void update()
   {

   }

   // render: lugar para dibujar el árbol en pantalla con Raylib.
   void render()
   {
      BeginDrawing ();

      ClearBackground (RAYWHITE);

      EndDrawing ();
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