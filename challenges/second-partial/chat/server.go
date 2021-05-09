// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	"log"
	"net"
	"os"
	"strings"
	"time"
)

//!+broadcaster
type client chan<- string // an outgoing message channel

var (
	// all incoming client messages
	entering = make(chan client)
	leaving  = make(chan client)
	messages = make(chan string)

	//mapa de conexiones, que usa el nombre de usuario como llave
	//guarda la conexion de cada usuario
	usuariosEnLinea map[string]net.Conn

	//mapa de horas de conexion, que usa el nombre de usuario como llave
	tiemposDeCon map[string]time.Time

	//nombre del administrador
	adiministrador string

	//booleano para manejar administrador
	faltaAdmin = false

	//Colored Outputs
	Reset  = "\033[0m"
	Red    = "\033[31m"
	Green  = "\033[32m"
	Yellow = "\033[33m"
	Blue   = "\033[34m"
	Purple = "\033[35m"
	Cyan   = "\033[36m"
	Gray   = "\033[37m"
	White  = "\033[97m"
)

//funcion tiempo, para el comando /time
func tiempo() string {
	time := time.Now().Format(time.Kitchen) //Se obtiene el tiempo en formato de 12 horas
	return Green + "SERVIDOR> Hora local en America/Mexico_City: " + strings.Trim(time, "\n") + Reset
}

//funcion usuarios, para el comando /users
func usuarios() string {
	out := ""

	out += Purple + "SERVIDOR > Usuarios conectados: \n"
	out += "#################################\n"

	//se recorre el mapa, agregando los usuarios al string de salida
	for user, _ := range usuariosEnLinea {
		out += user

		//si el usuario es administrador, se muestra despues de su nombre
		if user == adiministrador {
			out += " (Administrador)"
		}

		out += " - conectado desde: " + tiemposDeCon[user].Format(time.ANSIC) + "\n"
	}

	out += "##################################" + Reset

	return out
}

//funcion infoUaurio, para el comando /user
func infoUsuario(usuario string) string {
	out := ""
	out += Blue + "#########################################\n"

	//se valida que el usuario buscado este presente
	if _, ok := usuariosEnLinea[usuario]; ok {
		out += "Nombre de usuario: " + usuario + "\n"
		out += "IP: " + usuariosEnLinea[usuario].RemoteAddr().String() + "\n"
		out += "Conectado desde: " + tiemposDeCon[usuario].Format(time.ANSIC) + "\n"
		out += "#########################################" + Reset
		return out
	} else {
		return "SERVIDOR > el usuario <" + usuario + "> no existe o no esta conectado" //si no se encuentra, se regresa un string que informa eso
	}
}

//funcion remover, para el comando /kick
func remover(usuario, ban string) {
	//se verifica que el usuario que lo ejecute sea administrador
	if strings.Compare(usuario, adiministrador) != 0 {
		fmt.Fprintln(usuariosEnLinea[usuario], "SERVIDOR > Solo el administrador puede ejecutar este comando")
	} else {
		//se busca el usuario en el mapa
		if _, ok := usuariosEnLinea[ban]; ok {
			fmt.Fprintln(usuariosEnLinea[ban], Red+"SERVIDOR > El administrador te ha sacado por comportamiento inadecuado."+Reset)                       //se informa al usuario que lo banearon
			fmt.Println(Red + "SERVIDOR > El usuario [" + Cyan + ban + Red + "] ha sido removido" + Reset)                                                //se registra la accion en el server
			messages <- Red + "El usuario [" + Cyan + ban + Red + "] ha sido removido del server. Recuerden mantener un comportamiento adecuado." + Reset //se informa a todos

			//se cierra su conexion y se borra de los mapas
			usuariosEnLinea[ban].Close()
			delete(usuariosEnLinea, ban)
			delete(tiemposDeCon, ban)
		} else {
			fmt.Fprintln(usuariosEnLinea[usuario], "SERVIDOR > el usuario <"+ban+"> no existe o no esta conectado")
		}
	}
}

//funcion mensaje, para el comando /msg
func mensaje(origin, destino string, mensaje []string) {
	//se verifica que el destinatario este presente
	if _, ok := usuariosEnLinea[destino]; ok {
		//se imprime el mensaje en la conexion del destinario y remitente
		fmt.Fprintln(usuariosEnLinea[origin], Yellow+"(Le murmuras a "+destino+")> "+strings.Join(mensaje, " ")+Reset)
		fmt.Fprintln(usuariosEnLinea[destino], Yellow+"("+origin+" te murmura)> "+strings.Join(mensaje, " ")+Reset)
		return
	} else {
		fmt.Fprintln(usuariosEnLinea[origin], "SERVIDOR > el usuario <"+destino+"> no existe o no esta conectado")
	}
}

func broadcaster() {
	time.LoadLocation("Local")
	//se inicializan los mapas
	tiemposDeCon = make(map[string]time.Time)
	usuariosEnLinea = make(map[string]net.Conn)

	clients := make(map[client]bool) // all connected clients

	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range clients {
				cli <- msg
			}

		case cli := <-entering:
			clients[cli] = true

		case cli := <-leaving:
			delete(clients, cli)
			close(cli)
		}
	}
}

//!-broadcaster

//!+handleConn
func handleConn(conn net.Conn) {

	ch := make(chan string) // outgoing client messages

	//Se lee el usuario que manda el cliente
	buffer := make([]byte, 128)
	conn.Read(buffer)
	usuario := string(bytes.Trim(buffer, "\x00"))
	usuario = strings.Replace(usuario, "\n", "", 1)

	//Se asigna el usuario
	who := usuario

	//Se levanta la bandera de necesidad de Administrador
	if len(usuariosEnLinea) == 0 {
		faltaAdmin = true
	}

	//Se verifica que no exista el usuario
	if usuariosEnLinea[who] != nil {
		fmt.Fprintln(conn, "EL NOMBRE "+who+" YA ESTA OCUPADO. ELIGE OTRO.")
		close(ch)
		conn.Close()
		return
	}

	//Se agrega la conexion a mapa, con el nombre de llave
	usuariosEnLinea[who] = conn

	//se agrega la hora de conexion al mapa, con el nombre de llave
	tiemposDeCon[who] = time.Now().UTC()

	//mensajes de bienvenida
	fmt.Printf("SERVIDOR > Se ha unido el usuario [%s%s%s]\n", Cyan, who, Reset)
	fmt.Fprintln(conn, "SERVIDOR > Bienvenido al Sevidor IRC simple")
	fmt.Fprintln(conn, "SERVIDOR > Te has conectado a las: "+tiemposDeCon[who].Format(time.ANSIC))
	fmt.Fprintln(conn, "SERVIDOR > Tu usuario ["+Cyan+who+Reset+"] ha sido registrado correctamente")

	//se asigna un administrador
	if faltaAdmin {
		adiministrador = who
		fmt.Println("SERVIDOR > [" + Cyan + who + Reset + "] ahora es el administrador del server")
		fmt.Fprintln(conn, "SERVIDOR > Ahora eres el administrador del servidor!")
		faltaAdmin = false
	}

	go clientWriter(conn, ch)

	//se informa de la llegada de un nuevo usuario
	messages <- "SERVIDOR > Nuevo usuario conectado [" + Cyan + who + Reset + "]"
	entering <- ch

	input := bufio.NewScanner(conn)
	for input.Scan() {

		if len(input.Text()) > 0 {
			//si se trata de un comando, entra al switch case
			if string(input.Text()[0]) == "/" {

				comando := strings.Split(input.Text(), " ")
				funcion := comando[0]

				//switch case para los distintos comandos
				switch funcion {

				case "/msg":
					//validacion de formato correcto
					if len(comando) < 3 {
						ch <- "SERVIDOR > ARGUMENTOS INCORRECTOS, usa /msg <usuario> <mensaje>"
						continue
					}

					mensaje(who, comando[1], comando[2:])

				case "/users":
					//validacion de formato correcto
					if len(comando) != 1 {
						ch <- "SERVIDOR > Este comando no necesita argumentos, solo usa /users"
						continue
					} else {
						ch <- usuarios()
					}

				case "/kick":
					//validacion de formato correcto
					if len(comando) < 2 {
						ch <- "SERVIDOR > ARGUMENTOS INCORRECTOS, usa /kick <usuario>"
						continue
					}

					remover(who, comando[1])

				case "/time":
					//validacion de formato correcto
					if len(comando) != 1 {
						ch <- "SERVIDOR > Este comando no necesita argumentos, solo usa /time"
						continue
					} else {
						ch <- tiempo()
					}

				case "/user":
					//validacion de formato correcto
					if len(comando) != 2 {
						ch <- "SERVIDOR > ARGUMENTOS INCORRECTOS, usa /user <usuario>"
						continue
					} else {
						ch <- infoUsuario(comando[1])
					}
				default:
					//si no se detecta un caso, se informa al usuario
					ch <- "SERVIDOR > Comando no reconocido despues del caracter '/' "
				}

			} else {
				//envio de mensajes con nombre de usuario
				messages <- who + " > " + input.Text()
			}

		}
	}
	// NOTE: ignoring potential errors from input.Err()

	//mensajes de salida de usuario
	fmt.Println("SERVIDOR > [" + Cyan + who + Reset + "] se fue.")
	messages <- "SERVIDOR > [" + Cyan + who + Reset + "] dejo el chat. See you, space cowboy."

	//se elimina al usuario de los mapas
	delete(usuariosEnLinea, who)
	delete(tiemposDeCon, who)
	conn.Close()

	//se asigna un nuevo administrador
	if who == adiministrador {
		for user, _ := range usuariosEnLinea {
			adiministrador = user
			fmt.Println("SERVIDOR > [" + Cyan + user + Reset + "] ahora es el administrador")
			messages <- "SERVIDOR > [" + Cyan + user + Reset + "] es el nuevo admin, el antiguo [" + Cyan + who + Reset + "], se desconecto."
			break
		}
	}

}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

//!-handleConn

//!+main
func main() {

	//Validacion de entrada de suficientes elementos
	if len(os.Args) < 5 || len(os.Args) > 5 {
		fmt.Println(Red + "ERROR: ENTRADA NO ACEPTADA" + Reset + "SIGUE EL SIGUIENTE FORMATO:")
		fmt.Println(Green + "go run server.go -host <direccion de host> -port <puerto> " + Reset)
		os.Exit(1)

	}

	//Validacion de formato de entrada correcta
	if os.Args[1] != "-host" || os.Args[3] != "-port" {
		fmt.Println(Red + "ERROR: ENTRADA NO ACEPTADA" + Reset + " SIGUE EL SIGUIENTE FORMATO:")
		fmt.Println(Green + "go run server.go -host <direccion de host> -port <puerto> " + Reset)
		os.Exit(1)
	}

	fmt.Println("SERVIDOR > Inicializando...")

	host := flag.String("host", "localhost", "address")
	port := flag.String("port", os.Args[4], "listener")
	flag.Parse()

	listener, err := net.Listen("tcp", *host+":"+*port)

	if err != nil {
		log.Fatal(err)
	}

	go broadcaster()

	fmt.Println("SERVIDOR > Servidor IRC correctamente montado en " + *host + ":" + *port)
	fmt.Println("SERVIDOR > Servidor inicializado. Listo para recibir clientes.")

	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}

}

//!-main
