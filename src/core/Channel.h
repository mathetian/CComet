// Copyright (c) 2014 The CComet Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "Noncopyable.h"
using namespace utils;

#include "Subscriber.h"

namespace ccomet
{

class Channel : public Noncopyable
{
public:
    typedef map<string, string> Params;
public:
    /// Constructor
    Channel(string cname);

    /// Destructor
    virtual ~Channel();

public:
    /// Find a subscriber from this channel
    Subscriber *find(const string &name);

    /// Add a subscriber from this channel
    bool add(Subscriber *subscriber);

    /// Del(remove) a subscriber from this channel
    bool del(Subscriber *subscriber);

public:
    /// Add a message to history
    bool add(const string &msg);

    /// Format string, according to the type
    string format(Params &keys, string type);

public:
    /// Get All Messages
    string getHistory() const;

    /// Get Sub Messages
    ///
    /// @param seqid, from seqid to history_.size()(exclude)
    string getSubHistory(int seqid) const;

    /// Get the maxinum ID
    int    getMxID() const;

public:
    /// Send the `chat` data
    void sendChat(const string &message);

    /// Send the `sign` data
    void sendSign(const string &messag);

private:
    /// All subscribers in this channel
    map<string, Subscriber*> subscribers_;

    /// Messages history in this channel
    vector<string>           history_;

    /// Channel Name
    string                   cname_;
};

};

#endif