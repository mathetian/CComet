function CComet(config)
{
	var self = this;
	
	self.cb = 'ccomet_cb';
	self.sedid = 0;
	self.sub_url  = config.sub_url;
	self.pub_url  = config.pub_url;
	self.sign_url = config.sign_url;
	self.callback = config.callback;
	self.sign_status = 0;
	self.callbackflag = 0;
	
	window[self.cb] = function(msg)
	{
		console.log(typeof msg);
		var msg = JSON.parse(msg);
		var item = msg[0];
		console.log(msg);
		if(item.type == '404')
		{	
			alert(msg);
		}
		else if(item.type == '401')
		{
			alert(msg);
		}
		else if(item.type == 'pub')
		{

		}
		else
		{
			for(var item in msg)
				self.callback(msg[item]);
			seqid = parseInt(item[item.length-1].seqid)+1;
		}
	}

	self.sign = function(){
		self.log('sign in...');

		var url = self.sign_url + '&time=' + new Date().getTime();

		$.ajax({
			url: url,
			dataType: "jsonp",
			jsonpCallback: "ccomet_cb",
		});
	}

	self.sub = function(){
		self.log('subscriber...');

		var url = self.sub_url + self.seqid + '&time=' + new Date().getTime();
		
		$.ajax({
			url: url,
			dataType: "jsonp",
			jsonpCallback: "ccomet_cb",
		});
	}
	
	self.pub = function(content){
		self.log('publish...');

		var url = self.pub_url + content + '&time=' + new Date().getTime();
		console.log(url);
		$.ajax({
			url: url,
			dataType: "jsonp",
			jsonpCallback: "ccomet_cb",
		});
	}

	self.start = function()
	{
		if(self.callbackflag == 1) return;
		self.callbackflag = 1;
		
		if(self.sign_status == 0) self.sign();
		else self.sub();
		
		self.timechecker();
	}
	
	self.timechecker = function()
	{
		self.timer = setInterval(function(){
			self.start();
		}, 5000);
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

	self.start();
}
