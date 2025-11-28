/ඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞ
    UNIVERSIDAD NACIONAL AUTONOMA DE MEXICO
    FACULTAD DE ESTUDIOS SUPERIORES -ARAGON-

    Computadoras y programacion.
    (c) Aaron Roman Montiel Gutierrez, 323332996.

    Quiso decir: Programa principal de la aplicacion de la distancia de Levenstein.

ඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞ/
#include "stdafx.h"
#include <string.h>
#include "corrector.h"
#include <fstream>
#include <cctype>
#include <cstring>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <iterator>
// Función para comparación en bsearch
int str_cmp(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b);
}
struct Candidata {
    char palabra[TAMTOKEN];
    int peso;
};

struct CandidataComparator {
    bool operator()(const Candidata& a, const Candidata& b) const {
        if (a.peso != b.peso) return a.peso > b.peso;
        return strcmp(a.palabra, b.palabra) < 0;
    }
};
//Funciones publicas del proyecto
/*****************************************************************************************************************
    DICCIONARIO: Esta funcion crea el diccionario completo
    char *	szNombre				:	Nombre del archivo de donde se sacaran las palabras del diccionario
    char	szPalabras[][TAMTOKEN]	:	Arreglo con las palabras completas del diccionario
    int		iEstadisticas[]			:	Arreglo con el numero de veces que aparecen las palabras en el diccionario
    int &	iNumElementos			:	Numero de elementos en el diccionario
******************************************************************************************************************/
void Diccionario(char* szNombre, char szPalabras[][TAMTOKEN], int iEstadisticas[], int& iNumElementos)
{
    std::ifstream archivo(szNombre);
    if (!archivo) {
        iNumElementos = 0;
        return;
    }
    std::string content((std::istreambuf_iterator<char>(archivo)), std::istreambuf_iterator<char>());
    std::map<std::string, int> diccionario;
    char bufer[TAMTOKEN] = { 0 };
    int indice = 0;
    for (std::string::const_iterator it = content.begin(); it != content.end(); ++it) {
        char caracter = *it;
        if (isspace(static_cast<unsigned char>(caracter)) || caracter == ',' || caracter == '.' || caracter == ';' || caracter == '(' || caracter == ')' || caracter == '\r') {
            if (indice > 0) {
                bufer[indice] = 0;
                for (int i = 0; bufer[i]; i++) {
                    bufer[i] = tolower(static_cast<unsigned char>(bufer[i]));
                }
                diccionario[bufer]++;
                indice = 0;
            }
        }
        else {
            if (indice < TAMTOKEN - 1) {
                bufer[indice++] = caracter;
            }
        }
    }
    if (indice > 0) {
        bufer[indice] = 0;
        for (int i = 0; bufer[i]; i++) {
            bufer[i] = tolower(static_cast<unsigned char>(bufer[i]));
        }
        diccionario[bufer]++;
    }
    if (diccionario.empty()) {
        iNumElementos = 0;
        return;
    }
    iNumElementos = diccionario.size();
    std::map<std::string, int>::const_iterator ite = diccionario.begin();
    int idx = 0;
    for (; ite != diccionario.end(); ++ite) {
        strcpy_s(szPalabras[idx], TAMTOKEN, ite->first.c_str());
        iEstadisticas[idx] = ite->second;
        ++idx;
    }
}
/ඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞ
    ListaCandidatas: Esta funcion recupera desde el diccionario las palabras validas y su peso
    Regresa las palabras ordenadas por su peso
    char	szPalabrasSugeridas[][TAMTOKEN],	//Lista de palabras clonadas
    int		iNumSugeridas,						//Lista de palabras clonadas
    char	szPalabras[][TAMTOKEN],				//Lista de palabras del diccionario
    int		iEstadisticas[],					//Lista de las frecuencias de las palabras
    int		iNumElementos,						//Numero de elementos en el diccionario
    char	szListaFinal[][TAMTOKEN],			//Lista final de palabras a sugerir
    int		iPeso[],							//Peso de las palabras en la lista final
    int &	iNumLista)							//Numero de elementos en la szListaFinal
ඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞ/
void ListaCandidatas(
    char szPalabrasSugeridas[][TAMTOKEN],
    int iNumSugeridas,
    char szPalabras[][TAMTOKEN],
    int iEstadisticas[],
    int iNumElementos,
    char szListaFinal[][TAMTOKEN],
    int iPeso[],
    int& iNumLista)
{
    std::vector<Candidata> finales;
    for (int i = 0; i < iNumSugeridas; ++i) {
        char* clave = szPalabrasSugeridas[i];
        void* base = szPalabras;
        char* resultado = (char*)bsearch(clave, base, iNumElementos, TAMTOKEN, str_cmp);
        if (resultado) {
            int indice = (resultado - szPalabras[0]) / TAMTOKEN;
            Candidata c;
            memset(c.palabra, 0, TAMTOKEN);
            strcpy_s(c.palabra, TAMTOKEN, clave);
            c.peso = iEstadisticas[indice];
            finales.push_back(c);
        }
    }
    // Ordenar por peso descendente, luego alfabético ascendente
    std::sort(finales.begin(), finales.end(), CandidataComparator());
    iNumLista = finales.size();
    for (int k = 0; k < iNumLista; ++k) {
        strcpy_s(szListaFinal[k], TAMTOKEN, finales[k].palabra);
        iPeso[k] = finales[k].peso;
    }
}
/ඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞ
	ClonaPalabras: toma una palabra y obtiene todas las combinaciones y permutaciones requeridas por el metodo
	char *	szPalabraLeida,						// Palabra a clonar
	char	szPalabrasSugeridas[][TAMTOKEN], 	//Lista de palabras clonadas
	int &	iNumSugeridas)						//Numero de elementos en la lista
ඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞඞ/
void ClonaPalabras(
    char* szPalabraLeida,                       // Palabra a clonar
    char szPalabrasSugeridas[][TAMTOKEN],       //Lista de palabras clonadas
    int& iNumSugeridas)                         //Numero de elementos en la lista
{
    const char alfabeto[] = "abcdefghijklmnopqrstuvwxyz\361\341\351\355\363\372";
    const int longitud_alfabeto = sizeof(alfabeto) - 1;
    const int MAX_SUG = 4000;
    char szSugerencias[MAX_SUG][TAMTOKEN];
    int num = 0;
    char palabra[TAMTOKEN];
    strcpy_s(palabra, TAMTOKEN, szPalabraLeida);
    int longitud = strlen(palabra);
    // Añadir original
    strcpy_s(szSugerencias[num++], TAMTOKEN, palabra);
    // Eliminaciones
    for (int i = 0; i < longitud; ++i) {
        char temporal[TAMTOKEN];
        strncpy_s(temporal, TAMTOKEN, palabra, i);
        temporal[i] = '\0';
        strcat_s(temporal, TAMTOKEN, palabra + i + 1);
        if (temporal[0] != '\0') {
            strcpy_s(szSugerencias[num++], TAMTOKEN, temporal);
        }
    }
    // Trasposiciones
    for (int i = 0; i < longitud - 1; ++i) {
        char temporal[TAMTOKEN];
        strcpy_s(temporal, TAMTOKEN, palabra);
        char c = temporal[i];
        temporal[i] = temporal[i + 1];
        temporal[i + 1] = c;
        strcpy_s(szSugerencias[num++], TAMTOKEN, temporal);
    }
    // Sustituciones
    for (int i = 0; i < longitud; ++i) {
        for (int j = 0; j < longitud_alfabeto; ++j) {
            char temporal[TAMTOKEN];
            strcpy_s(temporal, TAMTOKEN, palabra);
            temporal[i] = alfabeto[j];
            strcpy_s(szSugerencias[num++], TAMTOKEN, temporal);
        }
    }
    // Inserciones
    for (int i = 0; i <= longitud; ++i) {
        for (int j = 0; j < longitud_alfabeto; ++j) {
            char temporal[TAMTOKEN];
            strncpy_s(temporal, TAMTOKEN, palabra, i);
            temporal[i] = alfabeto[j];
            temporal[i + 1] = '\0';
            strcat_s(temporal, TAMTOKEN, palabra + i);
            strcpy_s(szSugerencias[num++], TAMTOKEN, temporal);
        }
    }
    // Ordenar
    qsort(szSugerencias, num, TAMTOKEN, str_cmp);
    // Copiar a arreglo
    iNumSugeridas = num;
    for (int m = 0; m < num; ++m) {
        strcpy_s(szPalabrasSugeridas[m], TAMTOKEN, szSugerencias[m]);
    }

}
