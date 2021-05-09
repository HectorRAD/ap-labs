// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"os"
)

//!+
func main() {

	//Validacion de entrada de suficientes elementos
	if len(os.Args) < 5 || len(os.Args) > 5 {
		fmt.Println("\033[31mERROR: ENTRADA NO ACEPTADA. \033[0mSIGUE EL SIGUIENTE FORMATO:")
		fmt.Println("\033[32mgo run client.go -user <usuario> -server localhost:<puerto> \033[0m")
		os.Exit(1)

	}

	//Validacion de formato de entrada correcta
	if os.Args[1] != "-user" || os.Args[3] != "-server" {
		fmt.Println("\033[31mERROR: ENTRADA NO ACEPTADA. \033[0mSIGUE EL SIGUIENTE FORMATO:")
		fmt.Println("\033[32mgo run client.go -user <usuario> -server localhost:<puerto> \033[0m")
		os.Exit(1)
	}

	host := flag.String("server", os.Args[4], "server")
	usuario := flag.String("user", os.Args[2], "user")

	conn, err := net.Dial("tcp", *host)
	if err != nil {
		log.Fatal(err)
	}

	//Se imprime el usuario a la conexion, para poder asignarlo a la IP
	fmt.Fprintf(conn, *usuario+"\n")

	done := make(chan struct{})
	go func() {
		io.Copy(os.Stdout, conn) // NOTE: ignoring errors
		log.Println("done")
		done <- struct{}{} // signal the main goroutine
	}()
	mustCopy(conn, os.Stdin)
	conn.Close()
	<-done // wait for background goroutine to finish
	//os.Exit(1)

}

//!-

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}
