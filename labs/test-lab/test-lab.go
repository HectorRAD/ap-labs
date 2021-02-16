package main

import (
	"fmt"
	"os"
)

func main() {
  
  if len(os.Args) == 1{
    fmt.Println("ERROR")
    return
  }else if len(os.Args)==2 {
    fmt.Println("Hi, " +os.Args[1] + ".")
  }else{
    name := ""
    for i := 1; i < len(os.Args); i ++{
      name += os.Args[i] + " "
    }
    fmt.Println("Hi, " + name)
  }
  
	fmt.Println("Ready to roll")
}
