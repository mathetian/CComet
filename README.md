CComet
======
CComet is a tiny [Comet](http://en.wikipedia.org/wiki/Comet_(programming)) based on [EventServer](https://github.com/mathetian/EventServer) and it is developed and maintained by [Yulong Tian](http://github.com/mathetian). To be specific, `CComet` uses the http module provided by `EventServer`.

[Comet](http://en.wikipedia.org/wiki/Comet_(programming)) is a technology for push-notification server. It allows a web server to push data to a browser through HTTP protocal. In other atomsphere, such as android and windows phone, it can behaves [GCM](http://developer.android.com/google/gcm/‎) and [MPNS](http://msdn.microsoft.com/en-us/library/windows/apps/hh913756.aspx).

CComet can be used as the messaging server of many applications. To demonstrate the use of the library, I have provided a webchat demo in this project. In future, I will also provide sdks for android and windows phone apps.

Our work is mainly inspired by [icomet](https://github.com/ideawu/icomet) and I have taken many interesting idea from [ideawu](https://github.com/ideawu), both from blogs and source code. Thanks for `ideawu`. Thanks again for OSC (Open Source Community).

## Usage

### 1. Start the server.

```shell
git clone https://github.com/mathetian/CComet
cd CComet
make ccomet
cd bin
./comet
```

And run the web-demo in directory ```demo/web/```

### 2. Run the chat demo

1. Compile and start ccomet server
2. Drag and drop the file demos/web/chat.html into one web browser
3. Drag and drop the file demos/web/chat.html into another different web browser
4. Start chatting!

## Client API
We provide a client api of javascript in __CComet__. 
```Javascript
function CComet(config)
{
	var self = this;
	var sign_sta = false;
	self.seqid = 0;
	self.sub_url  = config.sub_url;
	self.pub_url  = config.pub_url;
	self.sign_url = config.sign_url;
	self.callback = config.callback;

	self.sign = function(){
	    /// For detail information, please seek for the ccomet.js
	}

	self.sub = function(){
	    /// For detail information, please seek for the ccomet.js
	}
	
	self.pub = function(content){
	    /// For detail information, please seek for the ccomet.js
	}

	self.sign();
}

```

## Performance

```
CComet   :   version 1.0
Date     :   Sunday June 20 20:24:47 2014
CPU      :   4 * Intel(R) Core(TM) i5-3317U CPU @ 1.70GHz
CPUCache :   3072 KB
G++      :   4.8.1
Demux    :   EPoll
Phy-Mem  :   3888 MB
```

In the benchmark of `EventServer`, we have found out the QPS and the memory cost of each connection of http module. We will discuss the cost of memory when we add a layer of ccomet.

| Connections        | VIRT  | RES   |
| -----------        | ----  | ---   | 
| 0                  | 24M   |  8M   |
| 10000              | 33M   |  10M  | 
| 50000              | 60M   |  36M  | 
| 100000             | 106M  |  81M  |  
| 200000             | 210M  |  162M | 

0.92 KB per connection `?`

__Discussion__ : Through our experiments, we have proved the stability of our system.


## Design Principle

1. Use jsonp to solve [CORS](en.wikipedia.org/wiki/Cross-origin_resource_sharing‎) which is short for Cross-Origin Resource Sharing, problems.
2. Long-polling instead of polling. To solve some problem in jsonp, I use a timeout 30 seconds, which is a reasonable value, in produce mode.
3. Compared with `icomet`, I remove authetication flow and support for iframe technology.

## CComet UML Graph

![CComet UML Graph](https://raw.githubusercontent.com/mathewes/blog-dot-file/master/CComet.png)

## Role in Produce Atmosphere

![ROLE](https://raw.githubusercontent.com/mathewes/blog-dot-file/master/CComet_flow.png)

## License

This client is licensed under the same license as EventServer. And this license will be restricted by the license of `icomet`.

## Todo List
1. Android & Windows Phone 8
2. Memory Pool
3. ...