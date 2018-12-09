#!/usr/bin/env phantomjs --ssl-protocol=any --ignore-ssl-errors=true --web-security=false

/*
	EXIT CODE
	0	report successful
	1	a web interaction step failed on user side
	2	wrong invocation
	3	page load failed
	4	a web interaction step failed on navigator side
	5	report failed
*/

var llDebug = 1, llStatus = 2, llError = 3, llMsg = 4;
var vlDebug = false;
var vlStatus = false;
var vlError = true;
var vlMsg = true;
var stderr = require("system").stderr;
var stdout = require("system").stdout;
var Glob = {};

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
		Glob.felhazon = phantom.args[i];
		Glob.gyariszam = phantom.args[i+1];
		Glob.meroallas = phantom.args[i+2];
		Glob.email = phantom.args[i+3];
		Glob.datum = phantom.args[i+4];
		i += 4;
	}
}

if(Glob.datum == undefined)
{
	stderr.write("Usage: nkmfoldgaz.js [--debug] [--verbose] <felhasználó-azonosító> <mérőóra-gyári-szám> <mérőállás> <email> <dátum>\n");
	phantom.exit(2);
}



var url_form = "https://www.nkmfoldgaz.hu/Ugyfelszolgalat/Havi-meroallas-kozlese-pub";
var loadInProgress = false;
var stepindex = 0;
var page = new WebPage();
page.settings.userAgent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/59.0.3071.90 Safari/537.36 Vivaldi/1.91.867.38";

function log(level, msg)
{
	if(level == llDebug && vlDebug || 
	   level == llStatus && vlStatus ||
	   level == llError && vlError ||
	   level == llMsg && vlMsg)
	{
		stderr.write(msg + "\n");
	}
}

page.onConsoleMessage = function(msg)
{
	log(llMsg, ">>> " + msg);
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

page.onError = function(msg, stack)
{
	log(llError, "ERROR: " + msg + "\nSTACK: " + JSON.stringify(stack));
};

page.onResourceRequested = function(requestData, networkRequest)
{
	if(!requestData.url.match(/^data:/))
	{
		log(llDebug, requestData.method+' '+requestData.url);
	}
	/*
	for(var i=0; i<requestData.headers.length; i++)
	{
		log(llDebug, requestData.headers[i].name + ': ' + requestData.headers[i].value);
	}
	log(llDebug, JSON.stringify(requestData));
	*/
};


page.open(url_form, function(status)
{
	if(status !== 'success')
	{
		log(llError, "ERROR " + status);
		phantom.exit(3);
	}
	else
	{
		var steps = [
			function(param)
			{
				/* Waiting iframe to load */
				if(!document.getElementById('WD2E'))
				{
					//console.log(document.querySelectorAll('html')[0].outerHTML);
					console.log("Waiting for #WD2E element...");
					return 0;
				}
				return 1;
			},
			function(param)
			{
				/* Filling in form fields by simulating typing on keyboard */
				
				function typein(field, str)
				{
					console.log("Typing in '" + str + "'...");
					field.focus();
					for(var pos = 0; pos < str.length; pos++)
					{
						for(var keyevent in {"keydown":1, "keypress":1, "input":1, "change":1, "keyup":1})
						{
							var ev = document.createEvent("KeyboardEvent");
							var keyCode = str.charCodeAt(pos);
							var charCode = str.charCodeAt(pos);
							ev.initKeyboardEvent(keyevent, true, true, window, null, false, false, false, false, keyCode, charCode);
							ev.key = str[pos];
							ev.code = charCode;
							field.dispatchEvent(ev);
						}
					}
					field.value = str;
				}
				
				typein(document.getElementById('WD17'), param.felhazon);
				typein(document.getElementById('WD1C'), param.gyariszam);
				typein(document.getElementById('WD21'), param.meroallas);
				typein(document.getElementById('WD26'), param.email);
				typein(document.getElementById('WD2B'), param.datum);
				
				return 1;
			},
			function(param)
			{
				/* Submit form by simulating user clicking on button */
				
				var WD2E = document.getElementById('WD2E');
				var ev = document.createEvent("MouseEvent");
				ev.initMouseEvent("click", true /* bubble */, true /* cancelable */, window, null, 0, 0, 0, 0, /* coordinates */ false, false, false, false, /* modifier keys */ 0 /*left*/, null);
				WD2E.dispatchEvent(ev);
				return 1;
			},
			function(param)
			{
				/* Wait response page to load */
				if(!document.getElementById('WD35'))
				{
					console.log("Waiting for #WD35 element...");
					return 0;
				}
				return 1;
			},
			function(param)
			{
				/* Echo response from server */
				var WD35 = document.getElementById('WD35');
				console.log(WD35.outerHTML);
				param.result = WD35.getAttribute('lsdata') + "\n" + WD35.innerHTML + "\n";
				return JSON.stringify({
					jump: 1,
					result: param,
				});
			}
		];

		timeout = 500;
		worker = function()
		{
			if(!loadInProgress)
			{
				/* Make sure we are in the correct frame */
				page.switchToFrame('wasframe');
				log(llDebug, "Frame: " + page.frameName + " \"" + page.frameTitle + "\" [" + page.frameUrl + "]");
				
				/* Execute next step in navigator's context */
				log(llDebug, "step " + stepindex);
				var ret = page.evaluate(steps[stepindex], Glob);
				if(typeof ret != "number")
				{
					ret = JSON.parse(ret);
					for(var key in Glob)
					{
						if(!ret.result.hasOwnProperty(key)) delete Glob[key];
					}
					for(var key in ret.result)
					{
						Glob[key] = ret.result[key];
					}
					//stderr.write(JSON.stringify(Glob));
					ret = ret.jump;
				}
				log(llDebug, "step " + stepindex + " returned: " + ret);
				
				if(ret === false)
				{
					phantom.exit(1);
				}
				else if(ret === undefined || ret === null)
				{
					phantom.exit(4);
				}
				stepindex += ret;
				
				if(stepindex >= steps.length)
				{
					log(llDebug, "complete");
					stdout.write(Glob.result);
					if(Glob.result.match(/POSITIVE/))
					{
						phantom.exit(0);
					}
					else
					{
						phantom.exit(5);
					}
				}
			}
			
			setTimeout(worker, timeout);
		}		
		setTimeout(worker, timeout);
	}
});
