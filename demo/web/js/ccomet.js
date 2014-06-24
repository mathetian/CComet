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
		self.log('sign in...');

		var url = self.sign_url + '&time=' + new Date().getTime();

		$.ajax({
			url: url,
			dataType: 'jsonp',
			timeout: 30000,
			success: function(msg)
			{
				try
				{
					var msg = JSON.parse(msg), item = msg[0];
					if(item.type == '404' || item.type == '401' || item.type == '402' || item.type == '403')
						self.log(item.type);
					else
					{
						self.log('sign in successfully');
						self.sign_sta = true;

						self.sub();
					}	
				}catch(err) { 
					self.log("error?");
					self.log(msg); self.log(err); 
				}
			},
			error: function (msg, textStatus, errorThrown)
			{
				self.log('failed sign: ' + msg);
				self.log(msg);
				self.log('status: ' + textStatus + ' ' + errorThrown);
			}
		});
	}

	self.sub = function(){
		self.log('subscriber...');

		var url = self.sub_url + self.seqid.toString() + '&time=' + new Date().getTime();
		console.log(url);
		$.ajax({
			url: url,
			dataType: 'jsonp',
			timeout: 30000,
			success: function(msg)
			{
				try
				{
					var msg = JSON.parse(msg), item = msg[0];
					if(item.type == '404' || item.type == '401' || item.type == '402' || item.type == '403')
						self.log(item.type);
					else
					{
						for(var item in msg)
							self.callback(msg[item]);
						if(msg.length == 1 && msg[0].type == 'SIGN')
						{}
						else self.seqid = parseInt(msg[msg.length-1].seqid) + 1;
						self.sub();
					}
				}catch(err) { self.log(msg); self.log(err); }
			},
			error: function (msg, textStatus, errorThrown)
			{
				self.log('failed sub: ');
				self.log(msg);
				self.log('status: ' + textStatus + ' ' + errorThrown);
			}
		});
	}
	
	self.pub = function(content){
		self.log('publish...');

		var url = self.pub_url + content + '&time=' + new Date().getTime();
		
		$.ajax({
			url: url,
			dataType: 'jsonp',
			timeout: 30000,
			success: function(msg)
			{
				try
				{
					var msg = JSON.parse(msg), item = msg[0];
					if(item.type == '404' || item.type == '401' || item.type == '402'|| item.type == '403')
						self.log(item.type);
					else
						self.log('pub successfully');
				}catch(err) { self.log(msg); self.log(err); }
			},
			error: function (msg, textStatus, errorThrown)
			{
				self.log('failed pub: ' + msg);
				self.log(msg);
				self.log(textStatus + ' ' + errorThrown);
			}
		});
	}

	self.log = function(){
		try{
			var v = arguments;
			var p = 'ccomet';
			var t = new Date().toTimeString().substr(0, 8);
			console.log(t, t, v);
		}catch(e){}
	}

	self.sign();
}
