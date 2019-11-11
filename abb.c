#include "abb.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pila.h"

/*-----------Structs-----------*/

typedef struct nodo_abb{
	struct nodo_abb_t* izq;
	struct nodo_abb_t* der;
	void* dato;
	const char* clave;
}nodo_abb_t;

typedef struct abb{
	nodo_abb_t* raiz;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir_dato;
	size_t cantidad;
}abb_t;

typedef struct abb_iter{
	abb_t* arbol;
	pila_t* pila;
};

/*-----------Funciones auxiliares-----------*/

void nodo_destruir(nodo_abb_t* nodo){
	free(nodo->clave);
	free(nodo);
}

void abb_iter_apilar_izquierdos(pila_t* pila, nodo_abb_t* nodo){
	if(!nodo){
		return;
	}
	else{
		pila_apilar(pila, nodo->izq);
		abb_iter_apilar_izquierdos(pila, nodo->izq);
	}
}
nodo_abb_t* nodo_abb_crear(const char *clave, void *dato){
	nodo_abb_t* nodo = malloc(sizeof(nodo_abb_t));
	if(nodo == NULL){
		return NULL;
	}
	char* copia_clave =  strdup(clave);
	nodo->clave = copia_clave;
	nodo->dato = dato;
	nodo->izq = NULL;
	nodo->der = NULL;
	return nodo;
}

/*-----------Primitivas del arbol-----------*/

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
	abb_t* abb = malloc(sizeof(abb_t));
	if(!abb){
		return NULL;
	}
	abb->raiz = NULL;
	abb->cmp = cmp;
	abb->destruir_dato = destruir_dato;
	abb->cantidad = 0;
	return abb;
}

bool colocar_nodo(abb_t *arbol,nodo_abb_t* nodo, nodo_abb_t* padre){
	int comparacion = arbol->cmp(nodo->clave, padre->clave);
	if(comparacion == 0){
		if(abb->destruir_dato != NULL){
			abb->destruir_dato(padre->valor);
		}
		padre->valor = nodo->valor;
		free(nodo->clave); //Free de la copia de la clave creada cuando creo el nodo
		return true;
	}
	if(comparacion < 1){
		if(nodo->izq == NULL){
			padre->izq = nodo;
			return true;
		}
		return colocar_nodo(arbol,nodo,padre->izq);
	}
	else{
		if(nodo->der == NULL){
			padre->der = nodo;
			return true;
		}
		return colocar_nodo(arbol,nodo,padre->der);
	}
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
	nodo_abb_t* nodo = nodo_abb_crear(const char *clave, void *dato);
	if(nodo == NULL){
		return false;
	}
	if(arbol->raiz == NULL){
		arbol->raiz = nodo;
		return true;
	}
	return colocar_nodo(arbol,nodo,arbol->raiz);
}

nodo_abb_t* buscar_nodo( const abb_t *arbol, const char *clave,nodo_abb_t* padre){
	if(padre == NULL){
		return NULL;
	}
	int comparacion = arbol->cmp(nodo->clave, padre->clave);
	if(comparacion == 0){
		return padre;
	}
	if(comparacion < 1){
		return buscar_nodo(arbol,clave, padre->izq);
	}
	return buscar_nodo(arbol,clave, padre->der);
}
nodo_abb_t* todo_izquierda(nodo_abb_t* nodo){
	if(nodo->izq == NULL){
		return nodo;
	}
	return todo_izquierda(nodo->izq)
}
nodo_abb_t* buscar_reemplazante(nodo_abb_t* reemplazado){
	nodo_abb_t* derecho = reemplazado->der;
	if(derecho->izq == NULL){
		return derecho;
	}
	return todo_izquierda(derecho);
}
	
void *abb_borrar(abb_t *arbol, const char *clave){
}

void *abb_obtener(const abb_t *arbol, const char *clave){
	nodo_abb_t* nodo = buscar_nodo(arbol,clave,arbol->raiz);
	if(nodo){
		return nodo->dato
	}
	return NULL;
}
bool abb_pertenece(const abb_t *arbol, const char *clave){
	return buscar_nodo(arbol,clave,arbol->raiz);  
}

size_t abb_cantidad(abb_t *arbol){
	return arbol->cantidad;
}

void _abb_destruir(abb_t* arbol, nodo_abb_t* nodo){
	if(!nodo){
		return;
	}
	_abb_destruir(arbol, nodo->izq);
	_abb_destruir(arbol, nodo->der);
	arbol->destruir_dato(nodo->dato);
	nodo_destruir(nodo);
}

void abb_destruir(abb_t *arbol){
	_abb_destruir(arbol, arbol->raiz);
}

/*-----------Iterador interno-----------*/

bool _abb_in_order(nodo_abb_t* nodo, bool visitar(const char *, void *, void *), void *extra){
    if(!nodo){
    	return NULL;
	}
    if(!_abb_in_order(nodo->izq, visitar, extra)){
    	return false;
    }
    if(!visitar(nodo->clave, nodo->dato, extra)){
    	return false;
    }
    if(!_abb_in_order(nodo->der, visitar, extra)){
    	return false;
    }
    return true;
}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra){
	_abb_in_order(arbol->raiz, visitar, extra);
}

/*-----------Iterador externo-----------*/

abb_iter_t *abb_iter_in_crear(const abb_t *arbol){
	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
	if(iter == NULL){
		return NULL;
	}
	iter->pila = pila_crear();
	if(iter->pila == NULL){
		return NULL;
	}
	if(arbol){
		iter->arbol = arbol;
	}
	abb_iter_apilar_izquierdos(iter->pila, iter->arbol->raiz); // Llamo a funcion auxiliar para apilar nodos y ramas izquierdas
	return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter){
	if(abb_iter_in_al_final(iter)){
		return false;
	}
	nodo_abb_t* nodo = pila_desapilar(iter->pila); // Desapilo el nodo actual
	if(!nodo){
		return false;
	}
	if(nodo->der){ // Si el nodo desapilado tenia un derecho, vuelvo a apilar sus hijos
		pila_apilar(iter->pila, nodo->der);
		abb_iter_apilar_izquierdos(iter->pila, nodo->der-izq);
	}
	return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
	if(abb_iter_in_al_final == true){
		return NULL;
	}
	nodo_abb_t* nodo = pila_ver_tope(iter->pila);
	return nodo->clave;
}

bool abb_iter_in_al_final(const abb_iter_t *iter){
	if(pila_esta_vacia(iter->pila) == false){
		return false
	}
	else{
		return true;
	}
}

void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter->pila);
	free(iter);
}
