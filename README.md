<h1 align="center">Welcome to DataTransformer 👋</h1>
<p>
</p>

> A network-based application that offers several different data transformation services to users
> The master server operates on sentence-like messages entered by the user, and uses TCP as its transport-layer protocol, for reliable data transfer with the client. The client's interaction involves connecting to the master server, entering a sentence of one or more words to be used as the source data, and then entering a loop for interaction with the server. Within the loop, the client can specify what data transformations are desired on the original sentence source data, and in what order. These requests may involve one or more data transformations, to be performed in the order specified. The master server then communicates with the micro-services via UDP to perform the composed data transformations on each word, prior to returning the final result data back to the client via TCP. Additional client commands can be sent to apply new transformations to the same original sentence source data. When the client is finished with all data transformation requests, the session with the master server ends.

> The micro-services are offered via UDP-based communication. Each of these micro-service have to be running on different ports. 

## Install 
```sh
compile the following on separate terminals: 
    g++ -o masterserver masterserver.cpp 
    g++ -o masterclient masterclient.cpp
    g++ -o identity identity.cpp
    g++ -o reverse reverse.cpp
    g++ -o upper upper.cpp
    g++ -o lower lower.cpp
    g++ -o caesar caesar.cpp 
    g++ -o yours yours.cpp

Start the master server and client: 
	./masterserver 
	./masterclient 

    Start the microservices 
	./identity 
	./reverse 
	./upper
	./lower
	./caesar
	./yours 
```

## Usage
```sh
    Go to the client terminals to communicate with the microservices 
```

## Test

```sh
This software was tested primarily on local host 
```

## Author

👤 **Osagie Omigie**

* Website: osagie.me

## Show your support

Give a ⭐️ if this project helped you!

***
_This README was generated with ❤️ by [readme-md-generator](https://github.com/kefranabg/readme-md-generator)_