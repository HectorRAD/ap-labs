// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 156.

// Package geometry defines simple types for plane geometry.
//!+point

package main

import (
	"fmt"
	"math"
	"math/rand"
	"os"
	"strconv"
	"time"
)

//Estructura de los puntos
type Point struct{ X, Y float64 }

//Punto en el origen (0,0)
var pOri Point

//getters para los atributos de cada punto
func (p Point) getX() float64 {
	return p.X
}

func (p Point) getY() float64 {
	return p.Y
}

//Generadores de un numero random entre -100 y 100 para X y Y
func makeX() float64 {
	return (-100) + rand.Float64()*(100-(-100))
}

func makeY() float64 {
	return (-100) + rand.Float64()*(100-(-100))
}

// traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.getX()-p.getX(), q.getY()-p.getY())
}

// same thing, but as a method of the Point type
func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.getX()-p.getX(), q.getY()-p.getY())
}

//!-point

//!+path

// A Path is a journey connecting the points with straight lines.
type Path []Point

// Distance returns the distance traveled along the path.
func (path Path) Distance() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			sum += path[i-1].Distance(path[i])
		}
	}

	sum += path[len(path)-1].Distance(path[0])
	return sum
}

//Funcion encargada de obtener la orientacion de un segmento formado por dos puntos
func orientation(p, q, r Point) int {
	val := ((q.getY()-p.getY())*(r.getX()-p.getX()) - (q.getX()-p.getX())*(r.getY()-q.getY()))
	if val == 0 {
		return 0
	}
	if val > 0 {
		return 1
	} else {
		return 2
	}
}

//funcion encargda de imprimir un punto
func printPoint(p Point) {
	fmt.Printf("    •(%v5, %v5)\n", p.getX(), p.getY())
}

//Insertion sort usado para ordenar el Path de tal forma que no
//conflictuen entre si al ordenarlos respecto al origen
func InsertionSort(path Path) {

	L := len(path)
	for i := 1; i < L; i++ {
		j := i
		for j > 0 && compare(&path[j], &path[j-1]) == 0 {
			path[j], path[j-1] = path[j-1], path[j]
			j -= 1
		}
	}
}

//Fucion comparadora de distancia entre dos puntos al origen
func compare(p1, p2 *Point) int {
	var o int = orientation(pOri, *p1, *p2)
	if o == 0 {
		if Distance(pOri, *p2) >= Distance(pOri, *p1) {
			return 0
		} else {
			return 1
		}
	}

	if o == 2 {
		return 0
	} else {
		return 1
	}
}

//Funcion encargada de generar los puntos de un Path
func fillPath(lados int, path Path) {
	for i := 0; i < lados; i++ {
		x := math.Round(makeX()*100) / 100
		y := math.Round(makeY()*100) / 100
		path[i] = Point{X: x, Y: y}
	}

	//Se ordena el path
	InsertionSort(path)
}

//Funcion encargada de imprimir el perimetro y todas las distancias que lo conforman
func printPerimeter(path Path, lados int) {

	fmt.Printf("\n- Figure's perimeter:\n")

	// Primera distancia
	fmt.Printf(" - %f", path[0].Distance(path[1]))
	for i := 2; i < lados; i++ {
		//Distancias medias
		fmt.Printf(" + %f", path[i-1].Distance(path[i]))

		//salto de linea en caso de ser una gran cantidad de distancias
		if i%10 == 0 {
			fmt.Printf("\n  ")
		}
	}
	// Distancia final
	fmt.Printf(" + %f", path[lados-1].Distance(path[0]))
	// Perimetro
	fmt.Printf(" = %f\n", path.Distance())
}

func main() {

	var args []string = os.Args
	lados, error := strconv.Atoi(args[1])

	if len(args) != 2 {
		fmt.Printf("INPUT NO ACEPTADO, SIGA EL FORMATO: ./geometry <number>\n")
	} else if lados <= 2 {
		fmt.Printf("INPUT NO ACEPTADO, LA FIGURA NECESITA MAS DE 2 LADOS\n")
	} else if error == nil {

		rand.Seed(time.Now().UnixNano())

		fmt.Printf("INPUT ACEPTADO\n")
		fmt.Printf("\n- Generating a [%d] sides figure\n", lados)

		// Generate points
		var path Path = make(Path, lados)
		fillPath(lados, path)

		fmt.Printf("\n- Figure's vertices:\n")

		for i := 0; i < lados; i++ {
			printPoint(path[i])
		}

		printPerimeter(path, lados)
	}

}
