function CComet(config)
{
	var self = this;
	
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
			dataType: "jsonp",
			timeout: 5000,
			success: function(msg)
			{
				try
				{
					var msg = JSON.parse(msg), item = msg[0];
					if(item.type == '404' || item.type == '401' || item.type == '402')
						self.log(item.type);
					else
					{
						for(var item in msg)
							self.callback(msg[item]);
						self.seqid = parseInt(msg[msg.length-1].seqid)+1;
					}
					self.sub();
				}catch(err) { self.log(msg); self.log(err); self.sign(); }
			},
			error: function (msg, textStatus, errorThrown)
			{
				self.log(msg);
				self.log(textStatus + ' ' + errorThrown);

				self.sign();
			}
		});
	}

	self.sub = function(){
		self.log('subscriber...');

		var url = self.sub_url + self.seqid.toString() + '&time=' + new Date().getTime();

		$.ajax({
			url: url,
			dataType: "jsonp",
			timeout: 30000,
			success: function(msg)
			{
				try
				{
					var msg = JSON.parse(msg), item = msg[0];
					if(item.type == '404' || item.type == '401' || item.type == '402')
						self.log(item.type);
					else
					{
						for(var item in msg)
							self.callback(msg[item]);
						self.seqid = parseInt(msg[msg.length-1].seqid)+1;
					}
				}catch(err) { self.log(msg); self.log(err); }
				self.sub();
			},
			error: function (msg, textStatus, errorThrown)
			{
				self.log(msg);
				self.log(textStatus + ' ' + errorThrown);
				self.sub();
			}
		});
	}
	
	self.pub = function(content){
		self.log('publish...');

		var url = self.pub_url + content + '&time=' + new Date().getTime();
		$.ajax({
			url: url,
			dataType: "jsonp",
			timeout: 5000,
			success: function(msg)
			{
				try
				{
					var msg = JSON.parse(msg), item = msg[0];
					if(item.type == '404' || item.type == '401' || item.type == '402')
						self.log(item.type);
					else
						self.log('pub successfully');
				}catch(err) { self.log(msg); self.log(err); }

			},
			error: function (msg, textStatus, errorThrown)
			{
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
			if(v.length == 1){
				console.log(t, p, v[0]);
			}else if(v.length == 2){
				console.log(t, p, v[0], v[1]);
			}else if(v.length == 3){
				console.log(t, p, v[0], v[1], v[2]);
			}else if(v.length == 4){
				console.log(t, p, v[0], v[1], v[2], v[3]);
			}else if(v.length == 5){
				console.log(t, p, v[0], v[1], v[2], v[3], v[4]);
			}else{
				console.log(t, p, v);
			}
		}catch(e){}
	}

	self.sign();
}
