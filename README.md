CComet
======
CComet is A tiny [Comet](http://en.wikipedia.org/wiki/Comet_(programming)) Based on [SealedServer](https://github.com/mathetian/SealedServer)

[Comet](http://en.wikipedia.org/wiki/Comet_(programming)) is a technology for push-notification server. It allows a web server to push data to a browser through HTTP protocal. In other atomsphere, such as android and windows phone, it can behaves [GCM](developer.android.com/google/gcm/‎) and [MPNS](http://msdn.microsoft.com/en-us/library/windows/apps/hh913756.aspx).

[SealedServer](https://github.com/mathetian/SealedServer) is a light-weighted network programming framework written by [mathetian](https://github.com/mathetian). To reduce redundant code, I remove some features, such as timer event and select selector, and add a tiny-http server based on it. 

Notice: the version of `SealedServer` library used by `CComet` is outdated.

CComet can be used as the messaging server of many applications. To demonstrate the use of the library, I have provided a webchat demo in this project. In future, I will also provide sdks for android and windows phone apps.

Our work is mainly inspired by [icomet](https://github.com/ideawu/icomet) and I have taken many interesting idea from [ideawu](https://github.com/ideawu)(from blogs and source code). Thanks for `ideawu`. Thanks again for open-source group.

## Usage

Start the server.

```shell
git clone https://github.com/mathetian/CComet
cd CComet
make ccomet
cd bin
./comet
```

And run the web-demo in directory ```demo/web/```

## API

### JavaScript

### Android

### Windows Phone 8

## Run the chat demo

1. Compile and start ccomet server
2. Drag and drop the file demos/web/chat.html into one web browser
3. Drag and drop the file demos/web/chat.html into another different web browser
4. Start chatting!

## Memory Usage

```
CComet   :   version 1.0
Date     :   Sunday April 13 20:24:47 2014
CPU      :   4 * Intel(R) Core(TM) i5-3317U CPU @ 1.70GHz
CPUCache :   3072 KB
G++      :   4.8.1
Demux    :   EPoll
Phy-Mem  :   3888 MB
```

1. First, we have tested the stability of our system. We will simulate one situation, different users connect to the server and after a short of period, they will be disconnected by themselves or by the server. At the same time, the total number of active connections won't be changed dynamically.

2. Basic Test for huge connection. 

| Active-Connection  | VIRT  | RES   |
| -----------        | ----  | ---   | 
| 1000               |       |       |
| 5000               |       |       |
| 10000              |       |       |   
| 50000              |       |       |
| 100000             |       |       |


| Connections        | VIRT  | RES   |
| -----------        | ----  | ---   | 
| 0                  | 24M   |  8M   |
| 10000              | 33M   |  10M  | 
| 50000              | 60M   |  36M  | 
| 100000             | 106M  |  81M  |  
| 200000             | 210M  |  162M | 

0.92 KB per connection `?`

## Bugs

There are many bugs in the project.

1. Write buffer overflow
2. Can't process specil character.
3. ...

## Todo lists

1. Client Simulators
2. Perfect HTTP Server
3. Memory Pool
3. ...

## Design Principle

1. Use jsonp to solve [CORS](en.wikipedia.org/wiki/Cross-origin_resource_sharing‎)(Cross-Origin Resource Sharing) problem.
2. Long-polling instead of polling. To solve some problem in jsonp, I use a timeout 30 seconds, which is a reasonable value, in produce mode.
3. Compared with `icomet`, I remove authetication flow and support for iframe technology.

## CComet UML Graph

![CComet UML Graph](https://raw.githubusercontent.com/mathewes/blog-dot-file/master/CComet.png)

## Role in Produce Atmosphere

![ROLE](https://raw.githubusercontent.com/mathewes/blog-dot-file/master/CComet_flow.png)

## License

This client is licensed under the same license as SealedServer. And this license will be restricted by the license of `icomet`.