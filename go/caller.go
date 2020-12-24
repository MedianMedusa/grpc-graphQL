package main

import (
	"context"
	"fmt"
	"os"

	"strings"
	"regexp"
	"google.golang.org/grpc"
	pb "./pbs"
)

const (
	address     = "127.0.0.1:5050"
	defaultName = "world"
)

func main() {
	//connection to the server.
	//fmt.Println("hello")
	//opts := []grpc.DialOption{grpc.WithInsecure(),}

	var conn *grpc.ClientConn

	conn, err := grpc.Dial(":50051", grpc.WithInsecure())
	if err != nil {
		fmt.Println("did not connect: ", err)
	}
	//fmt.Println("connected")
	defer conn.Close()

	client := pb.NewPhonesClient(conn)


	if len(os.Args) < 2 {
		fmt.Println("syntax: <this program> {(GraphQL request: phone = .., code = .., found, approved)}")
	return
	}

	var phone, code string
	var found, approved = false, false

	str := strings.Join(os.Args[1:], " ")
	reg := regexp.MustCompile(`{.*}`)
	str = reg.FindString(str)
	if len(str) > 0 {
		reg = regexp.MustCompile(`phone *= *[^ ,}]*`)
		phone = reg.FindString(str)
		if len(phone) > 0 {
			phone = strings.TrimSpace(phone[strings.Index(phone, "=") + 1: ])
			//fmt.Println("deb: phone =", phone)
		}

		reg = regexp.MustCompile(`code *= *[^ ,}]*`)
		code = reg.FindString(str)
		if len(code) > 0 {
			code = strings.TrimSpace(code[strings.Index(code, "=") + 1:])
		}

		if strings.Contains(str, "found") {
			found = true
		}
		if strings.Contains(str, "approved") {
			approved = true
		}
	}

	//phone := os.Args[1]
	//code := os.Args[2]

	response, err := client.SendRequest(context.Background(), &pb.Request{Phone: phone, Code: code})
	if err != nil {
                fmt.Println("did not response: ", err)
        }
	fmt.Println("{")
	if len(phone) > 0 {
		fmt.Printf("phone = %v", phone)
	}
	if len(code) > 0 {
		fmt.Printf(",\ncode = %v", code)
	}
	if found {
		fmt.Printf(",\nfound = %t", response.Found)
	}
	if approved {
		fmt.Printf(",\napproved = %t", response.Approved)
	}
	fmt.Printf("\n}\n")

	//fmt.Println("phone found - ", response.Found, ", phone approved - ", response.Approved)

}
