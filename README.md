CComet
======
CComet is A tiny [Comet](http://en.wikipedia.org/wiki/Comet_(programming)) Based on [SealedServer](https://github.com/mathetian/SealedServer)

[Comet](http://en.wikipedia.org/wiki/Comet_(programming)) is a technology for push-notification server. It allows a web server to push data to a browser through HTTP protocal.

[SealedServer](https://github.com/mathetian/SealedServer) is a light-weighted network programming framework written by [mathetian](https://github.com/mathetian). To reduce redundant code, I remove some features, such as timer event and select selector, and add a tiny-http server based on it.

CComet can be used as the messaging server of many applications. To demonstrate the use of the library, I have provided a webchat demo in this project. In near future, I will also provide sdks for android and windows phone apps.

Our work is mainly inspired by [icomet](https://github.com/ideawu/icomet). Thanks again for open-source group.

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

### JavaScript Library Usage

### Android Library Usage

### Windows phone Library Usage

## Run the chat demo

1. Compile and start ccomet server
2. Drag and drop the file demos/web/chat.html into one web browser
3. Drag and drop the file demos/web/chat.html into another different web browser
4. Start chatting!

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

## License

This client is licensed under the same license as SealedServer.
