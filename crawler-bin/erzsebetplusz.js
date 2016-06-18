#!/usr/bin/phantomjs --ssl-protocol=TLSv1

var llDebug = 1, llStatus = 2, llError = 3;
var vlDebug = false;
var vlStatus = false;
var vlError = true;
var stderr = require("system").stderr;
var Glob = {}

for(var i=0; i<phantom.args.length; i++)
{
	if(phantom.args[i] == '--debug') vlDebug = true;
	else if(phantom.args[i] == '--verbose') vlStatus = true;
	else if(phantom.args[i].match(/^-/))
	{
		stderr.write("Unknown option: "+phantom.args[i]+"\n");
		phantom.exit(2);
	}
	else
	{
		Glob.username = phantom.args[i];
		Glob.password = phantom.args[i+1];
		i += 1;
	}
}

if(Glob.password == undefined)
{
	stderr.write("Usage: erzsebetplusz.js [--debug] [--verbose] <kartya-utolso-8-szamjegye> <jelszo>\n");
	phantom.exit(2);
}


var url_login = "https://egyenleg.erzsebetutalvanyplusz.hu/cardholderlogin";
var loadInProgress = false;
var stepindex = 0;
var page = new WebPage();

function log(level, msg)
{
	if(level == llDebug && vlDebug || 
	   level == llStatus && vlStatus ||
	   level == llError && vlError)
	{
		stderr.write(msg + "\n");
	}
}

function output(text)
{
	console.log(text);
}


page.onConsoleMessage = function(msg)
{
	log(llDebug, ">>> " + msg);
};

page.onLoadStarted = function()
{
	loadInProgress = true;
	log(llStatus, "load started [" + page.url + "]");
};

page.onLoadFinished = function()
{
	loadInProgress = false;
	log(llStatus, "load finished [" + page.evaluate(function(){return document.title;}) + "]");
};

page.onResourceRequested = function(requestData, networkRequest)
{
	log(llDebug, requestData.method+' '+requestData.url);
	/*
	for(var i=0; i<requestData.headers.length; i++)
	{
		log(llDebug, requestData.headers[i].name + ': ' + requestData.headers[i].value);
	}
	log(llDebug, JSON.stringify(requestData));
	*/
};


page.open(url_login, function(status)
{
	if(status !== 'success')
	{
		log(llError, "ERROR " + status);
		phantom.exit(1);
	}
	else
	{
		var steps = [
			function(param)
			{
				//return document.getElementById('login_form').innerHTML;
				document.getElementById('username').value = param.username;
				document.getElementById('password').value = param.password;
				//document.getElementById('login_form').setAttribute('method','GET');
				var captcha_field = document.getElementById('g-recaptcha-response');
				captcha_field.parentElement.removeChild(captcha_field);
				document.getElementById('login_form').submit.click();
				return 1;
			},
			function()
			{
				var a = document.querySelectorAll('.alert');
				if(a && a.length)
				{
					for(var i=0; i<a.length; i++)
					{
						console.log(a[i].innerHTML.replace(/^\s*()/m, ''));
					}
					return false;
				}
				else
				{
					//console.log(document.querySelectorAll('html')[0].outerHTML);
				}
				return 1;
			}
		];

		worker = function()
		{
			if(!loadInProgress)
			{
				log(llDebug, "step "+stepindex);
				var ret = page.evaluate(steps[stepindex], Glob);
				log(llDebug, "step "+stepindex+" returned: "+ret);
				if(ret === false)
				{
					phantom.exit(1);
				}
				stepindex++;
			}
			
			if(stepindex < steps.length)
			{
				setTimeout(worker, 200);
			}
			else
			{
				log(llDebug, "complete");
				setInterval(checkReady, 200);
			}
		}		
		setTimeout(worker, 200);

		readBalance = function()
		{
			var balance = [];
			var name;
			var value;
			var a = document.querySelectorAll('.egyenlegWidgetHeaderTitleStyle');
			for(var i=0; i<a.length; i++)
			{
				//console.log(a[i].outerHTML);
				if(name == undefined)
				{
					name = a[i].innerText;
				}
				else
				{
					value = a[i].innerText;
					balance.push({name: name, value: value});
					name = undefined;
				}
			}
			return JSON.stringify(balance);
		}
		
		checkReady = function()
		{
			var balance = JSON.parse(page.evaluate(readBalance));
			if(balance && balance.length)
			{
				for(var i=0; i<balance.length; i++)
				{
					var zseb = balance[i].name.replace(/ zseb.*()/, '');
					var penz = balance[i].value.replace(/[^0-9 \.,]/g, '');
					output(zseb + ': ' + penz);
				}
				phantom.exit(0);
			}
		}
	}
});

