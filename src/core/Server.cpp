// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Server.h"
#include "HttpParser.h"

namespace ccomet
{

/// Singleton
Server& Server::Instance()
{
    static Server server_;
    return server_;
}

Server::Server()
{
}

Server::~Server()
{
}

STATUS1 Server::sign(HttpRequest *req, HttpResponse *rep)
{
    HttpParser *parser = req    -> getParser();
    Params      params = parser -> getParams();

    /// channel, sname, callback
    if(params.find("channel") == params.end() || params.find("sname") == params.end() 
            || params.find("callback") == params.end())
    {
        errorInParams();
        return ERRPARAM;
    }

    string cname    = params["channel"];
    string sname    = params["sname"];
    string callback = params["callback"];

    Channel     *channel = getChannel(cname);
    if(!channel) channel = newChannel(cname);

    /// Find the subscriber
    Subscriber *subscriber = channel -> find(sname);
    if(subscriber) {
        errorInOthers();
        return ERRDULPE;
    }
    
    /// Tell others in the same channel that somebody has logined in.
    string msg = channel -> format(keys, "SIGN");
    channel -> sendSign(msg);

    succeed(req, rep, 0);

    return SUCCEEED;
}

STATUS1 Server::publish(HttpRequest *req, HttpResponse *rep)
{
    HttpParser *parser = req -> getParser();
    Params      params = parser -> getParams();

    /// channel, sname, msg, callback
    if(params.find("channel") == params.end() || params.find("sname") == params.end()
            || params.find("msg") == params.end() || params.find("callback") == params.end())
    {
        errorInParams(req, rep);
        return ERRPARAM;
    }

    string cname = params["channel"];
    string sname = params["sname"];
    string msg   = params["msg"];
    string callback = params["callback"];

    Channel *channel = getChannel(cname);
    if(!channel) channel = newChannel(cname);

    string smsg = channel->format(keys, "MSG");
    channel -> sendChat(smsg);

    succeed(req, rep, 1);

    return SUCCEEED;
}

STATUS1 Server::subscribe(HttpRequest *req, HttpResponse *rep)
{
    HttpParser *parser = req -> getParser();
    Params      params = parser -> getParams();

    /// channel, sname, seqid, callback
    if(params.find("channel") == params.end() || params.find("sname") == params.end()
            || params.find("seqid") == params.end() || !is_int(params["seqid"]) || params.find("callback") == keys.end())
    {
        errorInParams();
        return ERRPARAM;
    }

    string cname     = keys["channel"];
    string sname     = keys["sname"];
    int seqid        = to_int(keys["seqid"]);
    string callback  = keys["callback"];

    Channel *channel = getChannel(cname);
    if(!channel) channel = newChannel(cname);

    /// Find the subscriber
    Subscriber *subscriber = channel -> find(sname);

    /// Can't duplicate
    if(subscriber) {
        errorInOthers(req, rep);
        return ERRDULPE;
    }

    /// Create the subscriber if not exist
    if(!subscriber) subscriber = new Subscriber(sname, seqid, callback, this, channel, req, rep);

    if(subscriber -> check() == false)
    {
        delete subscriber;
        subscriber = NULL;
    }
    
    return SUCCEEED;
}

STATUS1 Server::error(HttpRequest *req, HttpResponse *rep)
{
    /// TBD. We use the notfund here
    /// Anyone can add anything they need

    req -> notfund();
}

Channel* Server::getChannel(const string &name)
{
    if(channels_.find(name) == channels_.end())
        return NULL;

    return channels_[name];
}

Channel* Server::newChannel(const string &cname)
{
    Channel *channel= new Channel(cname);
    
    channels_[cname] = channel;
    
    return channel;
}

static void getlr(HttpRequest *req, string &lcallback, string &rcallback)
{
    map<string, string> params =  req -> getParams();

    if(params_.find("callback") != params_.end())
    {
        lcallback = params_["callback"] + "('[";
        rcallback = "]')";
    }
}

void     Server::errorInParams(HttpRequest *req, HttpResponse *rep)
{
    string lcallback; string rcallback;
    getlr(req, lcallback, rcallback);  

    rep -> addBody(lcallback + "{\"type\" : \"400\"}" + rcallback);    

    rep -> send();
}   

void     Server::errorInOthers(HttpRequest *req, HttpResponse *rep)
{
    string lcallback; string rcallback;
    getlr(req, lcallback, rcallback);

    rep -> addBody(lcallback + "{\"type\" : \"401\"}" + rcallback);    

    rep -> send();
}

void     Server::succeed(HttpRequest *req, HttpResponse *rep, int type)
{
    string lcallback; string rcallback;
    getlr(req, lcallback, rcallback);

    if(type == 0)
    {
        rep -> addBody(lcallback + "{\"type\" : \"sign in\"}" + rcallback);
    }
    else
    {
        rep -> addBody(lcallback + "{\"type\" : \"publish\"}" + rcallback);
    }

    rep -> send();
}


};